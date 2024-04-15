/*
 * Actors.cpp
 *
 *  Created on: Apr 9, 2024
 *      Author: liamk
 */


#include "Actors.h"
/*
 * Mailbox Implementation
 */
// & = reference to a message (m)
void MailBox::Enqueue(string id, ValueType vType, Val v, string objID){
  // give thread access and lock out the rest
  lock_guard<mutex> lock(m); // Note: Mutex is automatically unlocked when lock_guard goes out of scope
  Message* newMsg = nullptr;
  // Cast the message as an the appropriate child class
  switch (vType) {
    case ValueType::INTEGER:
        newMsg = new IntegerMessage(id, v.i, vType, objID);
        break;
    case ValueType::STRING:
        newMsg = new StringMessage(id, v.s, vType, objID);
        break;
    case ValueType::OBJECT:
        newMsg = new ObjectMessage(id, v.bA, vType);
        break;
  }
  if (newMsg != nullptr) {
    q.push(newMsg);
  }
}

Message * MailBox::Pop(){
  lock_guard<mutex> lock(m);
  if (q.empty()) {
    return nullptr;
  }
  Message* msg = q.front();
  q.pop();
  return msg;
}

/*
 * base actor implementation
 */

void BaseActor::processMessages(){
  std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
  while (running) {
     Message* msg = mailBox.Pop();
     if (msg != nullptr) {
       cout << "[" << id << "]" << ": found message!" << endl;
       auto it = messageHandlers.find(msg->id);
       if (it != messageHandlers.end()) {
         //second refers to the value which in this case is mapped to a void function with a msg param
         it->second(*msg);
       } else if (msg->id == "result"){
         cout << "[" << id << "]"<< " Received reply message with contents ";
         msg->printValue();
         cout << endl;
       }
       else {
         std::cout << "ERROR PROCESSING MESSAGE: UNKNOWN MSG" << std::endl;
       }
       if(msg->sentObjectID != "none"){
         // report back
         BaseActor* returnToActor = ActorList::getInstance().GetActor(msg->sentObjectID);
         if(returnToActor != nullptr){
           this->Send(*msg);
         }
       }
       delete msg;
     } else {
       // Sleep for a while if no messages in the mailbox
       //this_thread --> access the current thread.
       // cout << "BaseActor: no message; sleeping!" << endl;
       this_thread::sleep_for(chrono::milliseconds(100));
     }
   }
}

void BaseActor::Start(){
  running = true;
  // Create a new thread for the actor after waiting random time
  thread actorThread(&BaseActor::processMessages, this);
  // this_thread::sleep_for(chrono::milliseconds(rand() % 1000));
  // this only affect the thread calling the function  (base thread)
  cout << "[" << id << "]" << ": Started Thread!" << endl;
  actorThread.detach(); // Detach the thread to let it run independently
}

void BaseActor::Stop(){
  running = false;
}

/*
 * Child actor Implementations
 */

NumericActor::NumericActor(string newName){
  id = newName;
}

StringActor::StringActor(string newName){
  id = newName;
}


void NumericActor::PerformOperation(const Message& msg) {
  /*
   * maybe Multiply the current stored value
   */
  if (msg.type != ValueType::INTEGER)
    return;
  int readValue = static_cast<const IntegerMessage&>(msg).value;
  storedValue *= readValue;
  cout << "[" << id << "]" << " NumericActor Operation Resulted in Int: " << storedValue << endl;
}

void StringActor::PerformOperation(const Message& msg) {
  /*
   * maybe shuffle string around
   */
  if (msg.type != ValueType::STRING)
    return;
  string readValue = static_cast<const StringMessage&>(msg).value;
  storedValue = storedValue + " " + readValue;
  cout << "[" << id << "]" << " StringActor Operation Resulted in String: " << storedValue << endl;
}

void NumericActor::Store(const Message& msg){
  if (msg.type == ValueType::INTEGER) {
      storedValue = static_cast<const IntegerMessage&>(msg).value;
      cout << "[" << id << "]" << " NumericActor::Store : Stored value is now " << storedValue << endl;
  }
}

void StringActor::Store(const Message& msg){
  if (msg.type == ValueType::STRING) {
      storedValue = static_cast<const StringMessage&>(msg).value;
      cout << "[" << id << "]" << " StringActor::Store : Stored value is now " << storedValue << endl;
  }
}

void NumericActor::Send(const Message& msg){
  /*
   * formulate message, find target in list of actors
   * enqueue message into target mailbox
   */
  BaseActor* returnActor = ActorList::getInstance().GetActor(msg.sentObjectID);
  if(returnActor == nullptr){
    // we are sending down by creating a new actor
    string newName = "NewNumericActor" + to_string(rand());
    NumericActor* newActor = new NumericActor(newName);
    ActorList::getInstance().AddActor(newActor, newName);
    newActor->Start();
    // Send a message to the new actor
    ValueType vType = ValueType::INTEGER;
    Val v;
    v.i = (storedValue % 10) + 2;
    cout << "[" << id << "]" << " Sending New Numerical Actor the result of operation: (" << storedValue << " % 10) + 2" << endl;
    newActor->mailBox.Enqueue("store", vType, v, id);
  }else{
    // we are sending up by referencing past actor
    ValueType vType = ValueType::INTEGER;
    Val v;
    v.i = storedValue;
    returnActor->mailBox.Enqueue("result", vType, v, "none");
  }
}

void StringActor::Send(const Message& msg){
  /*
   * formulate message, find target in list of actors
   * enqueue message into target mailbox
   */
  BaseActor* returnActor = ActorList::getInstance().GetActor(msg.sentObjectID);
  if(returnActor == nullptr){
    // we are sending down by creating a new actor
    string newName = "NewStringActor" + to_string(rand());
    StringActor* newActor = new StringActor(newName);
    ActorList::getInstance().AddActor(newActor, newName);

    ValueType vType = ValueType::STRING;
    Val v;
    // just copy paste code for shuffle
    random_device rd;
    mt19937 g(rd());
    cout << "[" << id << "]" << " Sending New String Actor the result of Scrambling stored string: (" << storedValue << ")" << endl;
    shuffle(storedValue.begin(), storedValue.end(), rd);
    newActor->mailBox.Enqueue("store", vType, v, id);
  }else{
    // we are sending up by referencing past actor
    ValueType vType = ValueType::STRING;
    Val v;
    v.s = storedValue;
    returnActor->mailBox.Enqueue("result", vType, v, id);
  }
}

void ActorList::AddActor(BaseActor * actor, string name){
  if (actorList.find(name) == actorList.end()) { // eclipse hates map, don't worry
    cout << "ActorList::AddActor : added Actor {" << name << "}" << endl;
    actorList[name] = actor;
  }else{
    cout << "ActorList::AddActor : Actor with name {" << name << "} already exists" << endl;
    return;
  }
}

void ActorList::RemoveActor(string key){
  if (actorList.find(key) == actorList.end()) { // eclipse hates map, don't worry
    cout << "ActorList::RemoveActor : Cannot find name" << endl;
    return;
  }
  actorList.erase(key);
}

BaseActor * ActorList::GetActor(string key) {
  auto it = actorList.find(key);
  return it != actorList.end() ? it->second : nullptr;
}

