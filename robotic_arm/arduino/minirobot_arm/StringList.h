#ifndef STRING_LIST_H
#define STRING_LIST_H
#include <Arduino.h>

class StringListNode {
public:
  StringListNode(char *value, StringListNode* prev, StringListNode* next);
  StringListNode(char *value);
  ~StringListNode();
  void setPrevious(StringListNode* prev);
  void setNext(StringListNode* next);
  StringListNode* getPrevious();
  StringListNode* getNext();
  char* getValue();
private :
  char* value;
  StringListNode* previous;
  StringListNode* next;
};

class StringList {
public:
  StringList();
  void addTail(StringListNode *);
  void addTail(char* value);
  char* getForwardValue();
  char* getReverseValue();
  void reset();
  void clear();
private:
  bool isAtEnd;
  StringListNode *current;
  StringListNode *head;
  StringListNode *tail;
};
#endif
