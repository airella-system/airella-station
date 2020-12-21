#pragma once

#include <Arduino.h>

struct Node {
  String* value = nullptr;
  Node* next = nullptr;
};

class List {
public:
  List() {};
  ~List() {};
  void add(const String& data);
  String* getLast() const;
  bool isEmpty() const;
  String* pop();
private:
  Node* list = nullptr;
  Node* lastItem = nullptr;
};
