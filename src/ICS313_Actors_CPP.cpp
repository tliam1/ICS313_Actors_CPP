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

  // Start the actor threads
  numericActor.Start();
  stringActor.Start();

  // Create an actor list
  ActorList actorList;

  // Add actors to the actor list
  actorList.AddActor(&numericActor, "numeric_actor");
  actorList.AddActor(&stringActor, "string_actor");

  // Enqueue test messages
  Val v1;
  Val v2;
  v1.i = 10;
  v2.s = "hello";
  numericActor.mailBox.Enqueue("numeric_actor", ValueType::INTEGER, v1);
  stringActor.mailBox.Enqueue("string_actor", ValueType::STRING, v2);

  // Wait for a while to allow actors to process messages
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // Stop the actor threads
  numericActor.Stop();
  stringActor.Stop();

  return 0;
}
