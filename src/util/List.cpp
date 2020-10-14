#include "util/List.h"

void List::add(String& data) {
  Node* newNode = new Node;
  newNode->value = new String(data);
  if(lastItem == nullptr) {
    list = newNode;
    lastItem = list;
  }
  else {
    lastItem->next = newNode;
    lastItem = newNode;
  }
}

String* List::getLast() {
  if(lastItem == nullptr) return nullptr;
  return lastItem->value;
}

bool List::isEmpty() {
  return list == nullptr;
}

String* List::pop() {
  if(list == nullptr) return NULL;
  String* name = list->value;
  Node* newStart = list->next;
  delete list;
  list = newStart;
  return name;
}

void MultiValueList::add(MultiValueNode* newNode) {
  if(lastItem == nullptr) {
    list = newNode;
    lastItem = list;
  }
  else {
    lastItem->next = newNode;
    lastItem = newNode;
  }
}

MultiValueNode* MultiValueList::getLast() {
  if(lastItem == nullptr) return nullptr;
  return lastItem;
}

bool MultiValueList::isEmpty() {
  return list == nullptr;
}

MultiValueNode* MultiValueList::pop() {
  MultiValueNode* firstItem = list;
  delete list;
  list = firstItem->next;
  return firstItem;
}

void MultiValueList::clear(MultiValueNode* node, unsigned int valuesCount) {
  for(int i = 0; i < valuesCount; ++i) {
    delete node->values[i];
  }
  delete node->values;
  delete node;
}