/*
 * Actors.h
 *
 *  Created on: Apr 8, 2024
 *      Author: liamk
 */
#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <map>
#include <thread>
/*
 * Each instantiation and full specialization of
 * std::atomic<> represents a type that different threads can
 * simultaneously operate on (their instances), without raising undefined behavior
 */
#include <atomic>
/*
 * C++ header that provides a collection of types and functions to work with time.
 * It is a part of the C++ Standard Template Library (STL) and it’s included in C++11 and later versions.
 * <chrono> provides three main types of clocks: system_clock, steady_clock, and high_resolution_clock.
 * These clocks are used to measure time in various ways.
 */
#include <chrono>


#ifndef ACTORS_H_
#define ACTORS_H_

using namespace std;

class Message;
class IntegerMessage;
class StringMessage;
class ObjectMessage;
class MailBox;
#include "MailBox.h" // Include MailBox after forward declaration



/*
 *
 */
class BaseActor {
public:
  /*
   * each actor has it's own mailbox
   */
  MailBox mailBox;
  // a thread reading this value will get a value before being changed or the value after, avoids errors
  // turns out threads wont be affecting this sooooo
  atomic<bool> running; // flag to control the actor thread
  virtual void PerformOperation(const Message& msg) = 0;
  virtual void Store(const Message& msg) = 0;
  virtual void Send() = 0;
  void Start();
  void Stop();
  virtual void processMessages();

  BaseActor() : running(false) {}
  virtual ~BaseActor() {}
};

class NumericActor : public BaseActor {
private:
  int storedValue = 0;
  string id;
public:
  NumericActor(string newName);
  void PerformOperation(const Message& msg) override;
  void Store(const Message& msg) override;
  void Send() override;
};

class StringActor : public BaseActor {
private:
  string id;
  string storedValue = "";

public:
  StringActor(string newName);
  void PerformOperation(const Message& msg) override;
  void Store(const Message& msg) override;
  void Send() override;
};


/*
 * A queue of messages. Each message has a string identifying the message and a value to use with that message.
 * Values can be integers, strings, or references to other objects.
 * Enqueueing and dequeueing must be thread safe.
 */

/*
 * Store the value to use later.
 * Perform some calculations on numbers or strings.
 * Send messages either to the object in the message or to another object it received in a previous message.
 */
class Message{
public:
  string id;
  ValueType type;
  virtual void printValue() = 0;
  virtual ~Message() {}
};

class IntegerMessage : public Message {
public:
  int value;
  IntegerMessage(string id, int value, ValueType vType) : value(value) {
    this->id = id;
    this->type = vType;
  }
  void printValue() override {
    cout << "Integer value: " << value << endl;
  }
  ~IntegerMessage() {}
};

class StringMessage : public Message {
public:
  string value;
  StringMessage(string id, string value, ValueType vType) : value(value) {
    this->id = id;
    this->type = vType;
  }
  void printValue() override {
    cout << "String value: " << value << endl;
  }
  ~StringMessage() {}
};

class ObjectMessage : public Message {
public:
  BaseActor* value; // TODO: Change to a pointer to a real object later :) (class not made yet -- > actor class)
  ObjectMessage(string id, BaseActor* value, ValueType vType) : value(value){
    this->id = id;
    this->type = vType;
  }
  void printValue() override {
    cout << "Object type: " << endl;
  }
  ~ObjectMessage(){}
};

class ActorList{
public:
  map<string ,BaseActor *> actorList;
  void AddActor(BaseActor * actor, string name);
  void RemoveActor(string key);
};


ActorList * GetActorList();

#endif /* ACTORS_H_ */
