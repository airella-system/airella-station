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
  void add(String& data);
  String* getLast();
  bool isEmpty();
  String* pop();
private:
  Node* list = nullptr;
  Node* lastItem = nullptr;
};

class MultiValueNode {
public:
  MultiValueNode(unsigned int valuesCount) {
    values = new String*[valuesCount];
  }
  String** values;
  MultiValueNode* next = nullptr;
};

class MultiValueList {
public:
  MultiValueList() {};
  ~MultiValueList() {};
  void add(MultiValueNode* node);
  MultiValueNode* getLast();
  bool isEmpty();
  MultiValueNode* pop();
  static void clear(MultiValueNode* node, unsigned int valuesCount);
private:
  MultiValueNode* list = nullptr;
  MultiValueNode* lastItem = nullptr;
};
