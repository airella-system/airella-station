#include "util/List.h"
#include "maintenance/Logger.h"

void List::add(const String& data) {
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

String* List::getLast() const {
  if(lastItem == nullptr) return nullptr;
  return lastItem->value;
}

bool List::isEmpty() const {
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
