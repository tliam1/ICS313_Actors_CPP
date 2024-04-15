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
#include <algorithm>
#include <random>

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
#include <unordered_map>
#include <functional> // function

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
  // quick lookup for message ID's
  /*
   * function<void(const Message&)> can hold any function or function object that matches the specifications
   * In this case, it takes a const Message& argument and returns nothing
   */
  unordered_map<string, function<void(const Message&)>> messageHandlers;

  // a thread reading this value will get a value before being changed or the value after, avoids errors
  // turns out threads wont be affecting this sooooo
  atomic<bool> running; // flag to control the actor thread
  virtual void PerformOperation(const Message& msg) = 0;
  virtual void Store(const Message& msg) = 0;
  virtual void Send(const Message& msg) = 0;
  void Start();
  void Stop();
  virtual void processMessages();

protected:
  string id;
  int childIndexer;

  BaseActor() : running(false), childIndexer(0) {
    messageHandlers["store"] = [this](const Message& msg) { this->Store(msg); };
    messageHandlers["send"] = [this](const Message& msg) { this->Send(msg); };
    messageHandlers["op"] = [this](const Message& msg) { this->PerformOperation(msg); };
  }
  virtual ~BaseActor() {}
};

class NumericActor : public BaseActor {
private:
  int storedValue = 0;
public:
  NumericActor(string newName);
  void PerformOperation(const Message& msg) override;
  void Store(const Message& msg) override;
  void Send(const Message& msg) override;
};

class StringActor : public BaseActor {
private:
  string storedValue = "";

public:
  StringActor(string newName);
  void PerformOperation(const Message& msg) override;
  void Store(const Message& msg) override;
  void Send(const Message& msg) override;
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
  string sentObjectID;
  virtual void printValue() = 0;
  virtual ~Message() {}
};

class IntegerMessage : public Message {
public:
  int value;
  IntegerMessage(string id, int value, ValueType vType, string objID) : value(value) {
    this->id = id;
    this->type = vType;
    this->sentObjectID = objID;
  }
  void printValue() override {
    cout << value << endl;
  }
  ~IntegerMessage() {}
};

class StringMessage : public Message {
public:
  string value;
  StringMessage(string id, string value, ValueType vType, string objID) : value(value) {
    this->id = id;
    this->type = vType;
    this->sentObjectID = objID;
  }
  void printValue() override {
    cout << value << endl;
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
    static ActorList& getInstance() {
      static ActorList instance;
      return instance;
    }

    void AddActor(BaseActor* actor, string name);
    void RemoveActor(string key);
    BaseActor* GetActor(string key);
    map<string, BaseActor*> actorList;

private:
  // private to prevent instantiation
  ActorList() {}
  // Make copy constructor and assignment operator private to prevent cloning
  // deleting copy constructor
  // https://www.geeksforgeeks.org/implementation-of-singleton-class-in-cpp/
  ActorList(const ActorList&) = delete;
  ActorList& operator=(const ActorList&) = delete;
};


#endif /* ACTORS_H_ */
