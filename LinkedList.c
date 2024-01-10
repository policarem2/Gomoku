/* File name:   LinkedList.c
   Description: the implementation of LinkedList
*/

#include "LinkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

List *ListCreate() {
  List *list = (List *)malloc(sizeof(List));
  list->first = NULL;
  list->last = NULL;
  list->size = 0;

  list->cursor = NULL;
  return list;
}

// NOTE: this function may cause memory leaks
//       if the item in a node is linked to any data on the heap
//       the data would not freed by this function.
void ListDestroy(List *this) {
  Node *node = this->first;
  while (node != NULL) {
    Node *next = node->next;
    printf("item@%p, node@%p to be freed\n", node->item, node);
    free(node->item);
    free(node);
    node = next;
  }
  free(this);
}

Node *NodeCreate(void *item) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->item = item;
  node->next = NULL;

  return node;
}

void ListInsert(List *this, void *item) {
  Node *node = NodeCreate(item);

  node->next = this->first;
  this->first = node;
  if (this->size == 0) {
    this->last = node;
  }
  this->size++;
}

void ListFirst(List *this) { this->cursor = this->first; }

void *ListGetNext(List *this) {
  void *result = NULL;
  if (this->cursor != NULL) {
    result = this->cursor->item;
    this->cursor = this->cursor->next;
  }
  return result;
}
void ListAppend(List *this, void *item) {
  Node *node = NodeCreate(item);
  if(this->size==0){
    this->last = node;
  }
  else{
     Node *lastNode = this->last;
     lastNode->next = node;
     this->last = node;
  }
  if (this->size == 0) {
    this->first = node;
  }
  this->size++;
}

void *ListDelete(List *this) {  
  void *result = NULL; 
  if (this->size == 0) {
    return result;
  } 
  else {
    Node *node = this->first;
    this->first = node->next;
    result= node->item;
    free(node);
    this->size--;
    return result;
  }
}

void *ListGetAt(List *this, int index) {
  void *result = NULL;  
  if (this->size < index) {
    return result;
  }
  int counter = 0;
  ListFirst(this);
  while (counter <= index) {
    result = this->cursor->item;
    this->cursor = this->cursor->next;
    counter++;
  }
  return result;
}

