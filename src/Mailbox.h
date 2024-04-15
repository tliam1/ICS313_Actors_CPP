/*
 * Mailbox.h
 *
 *  Created on: Apr 13, 2024
 *      Author: liamk
 */

#ifndef MAILBOX_H_
#define MAILBOX_H_
#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include "Actors.h"
class BaseActor;
enum class ValueType { INTEGER, STRING, OBJECT };
/*
 * used for the mailbox implementation
 */
struct Val{
  int i = -1;
  string s;
  BaseActor * bA = nullptr;
};


class MailBox{
public:
  queue<Message *> q;
  mutex m;
  Message * Pop();
  void Enqueue(string id, ValueType vType, Val v, string objID);
};


#endif /* MAILBOX_H_ */
