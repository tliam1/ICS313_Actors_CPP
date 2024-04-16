//============================================================================
// Name        : ICS313_Actors_CPP.cpp
// Author      : Liam Tapper
// Version     :
// Copyright   : Your copyright notice
// Description : Actors Architecture in C++, Ansi-style
//============================================================================

#include <iostream>
#include "Actors.h"
using namespace std;

int main() {
  // set seed for random start time for threads
  srand(time(0));


  // Create instances of actors with their own mailboxes
  NumericActor numericActor = NumericActor("numeric_actor");
  StringActor stringActor = StringActor("string_actor");
  SquareOpActor squareActor = SquareOpActor("square_actor");
  // Enqueue test messages
  Val v1;
  Val v2;
  v1.i = 7;
  v2.s = "hello";
  numericActor.mailBox.Enqueue("store", ValueType::INTEGER, v1, "none");
  stringActor.mailBox.Enqueue("store", ValueType::STRING, v2, "none");
  v1.i = 22;
  v2.s = "world";
  numericActor.mailBox.Enqueue("op", ValueType::INTEGER, v1, "none");
  numericActor.mailBox.Enqueue("send", ValueType::INTEGER, v1, "none");
  stringActor.mailBox.Enqueue("op", ValueType::STRING, v2, "none");
  v1.i = 6;
  squareActor.mailBox.Enqueue("store", ValueType::INTEGER, v1, "none");
  squareActor.mailBox.Enqueue("op", ValueType::INTEGER, v1, "none");
  squareActor.mailBox.Enqueue("send", ValueType::INTEGER, v1, "none");

  // Add actors to the actor list in the singleton class
  ActorList::getInstance().AddActor(&numericActor, "numeric_actor");
  ActorList::getInstance().AddActor(&stringActor, "string_actor");
  ActorList::getInstance().AddActor(&squareActor, "square_actor");

  // Start the actor threads
  numericActor.Start();
  stringActor.Start();
  squareActor.Start();


  // Wait for a while to allow actors to process messages
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // Stop the actor threads
  auto& actorList = ActorList::getInstance().actorList;
  for (const auto& pair : actorList) {
    pair.second->Stop();
  }

  return 0;
}
