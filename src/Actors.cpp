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
void MailBox::Enqueue(string id, ValueType vType, Val v){
  // give thread access and lock out the rest
  lock_guard<mutex> lock(m); // Note: Mutex is automatically unlocked when lock_guard goes out of scope
  Message* newMsg = nullptr;
  // Cast the message as an the appropriate child class
  switch (vType) {
    case ValueType::INTEGER:
        newMsg = new IntegerMessage(id, v.i, vType);
        break;
    case ValueType::STRING:
        newMsg = new StringMessage(id, v.s, vType);
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
       cout << "BaseActor: found message!" << endl;
       PerformOperation(*msg);
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
  cout << rand() % 1000 << endl;
  thread actorThread(&BaseActor::processMessages, this);
  // this_thread::sleep_for(chrono::milliseconds(rand() % 1000));
  // this only affect the thread calling the function  (base thread)
  cout << "BaseActor: Started Thread!" << endl;
  actorThread.detach(); // Detach the thread to let it run independently
}

void BaseActor::Stop(){

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
   * maybe square the current stored value
   */
  NumericActor::Store(msg);
}

void StringActor::PerformOperation(const Message& msg) {
  /*
   * maybe shuffle string around
   */
  StringActor::Store(msg);
}

void NumericActor::Store(const Message& msg){
  if (msg.type == ValueType::INTEGER) {
      storedValue = static_cast<const IntegerMessage&>(msg).value;
      cout << "NumericActor::Store : Stored value is now " << storedValue << endl;
  }
}

void StringActor::Store(const Message& msg){
  if (msg.type == ValueType::STRING) {
      storedValue = static_cast<const StringMessage&>(msg).value;
      cout << "StringActor::Store : Stored value is now " << storedValue << endl;
  }
}

void NumericActor::Send(){
  /*
   * formulate message, find target in list of actors
   * enqueue message into target mailbox
   */
}

void StringActor::Send(){
  /*
   * formulate message, find target in list of actors
   * enqueue message into target mailbox
   */
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

ActorList * GetActorList(){
  /*
   * have a reference to some actor list
   */
  return nullptr;
}
