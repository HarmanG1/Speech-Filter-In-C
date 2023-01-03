#include "ll.h"
#include "node.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t seeks; // Number of seeks performed.
uint32_t links; // Number of links traversed.

struct LinkedList {
  uint32_t length;
  Node *head; // Head sentinel node .
  Node *tail; // Tail sentinel node .
  bool mtf;
};
// Sets up the linked list
LinkedList *ll_create(bool mtf) {
  LinkedList *ll = (LinkedList *)malloc(sizeof(LinkedList));
  if (ll != NULL) { // checks if allocation was not a failure
    ll->length = 0;
    ll->mtf = mtf;
    // sets the various node values and positions
    ll->head = node_create(NULL, NULL);
    ll->tail = node_create(NULL, NULL);
    ll->head->next = ll->tail;
    ll->tail->prev = ll->head;
    ll->head->prev = NULL;
    ll->tail->next = NULL;
  }
  return ll;
}
// deletes the linked list
void ll_delete(LinkedList **ll) {
  if (*ll != NULL) { // makes sure its not null
    while ((*ll)->head !=
           NULL) { // loops through the linked list's nodes and clears them
      Node *n = (*ll)->head->next;
      node_delete(&(*ll)->head);
      (*ll)->head = n;
    }
  }
  free(*ll);
  *ll = NULL;
}
// loops through ll to find number of nodes
uint32_t ll_length(LinkedList *ll) {
  uint32_t len = 0;
  Node *n = ll->head->next;
  while (n != ll->tail) {

    len++;
    n = n->next;
  }
  return len;
}
// self made string length function
static int strlengt(char *str) {
  int x = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    x++;
  }
  return x;
}
// self made str comparison function
static bool eqstr(char *org, char *comp) {
  if (strlengt(org) != strlengt(comp)) {
    return false;
  }
  for (int i = 0; org[i] != '\0'; i++) {
    if (comp[i] != org[i]) {
      return false;
    }
  }
  return true;
}
// checks if node is in the list
Node *ll_lookup(LinkedList *ll, char *oldspeak) {
  seeks++;
  Node *n = ll->head->next;
  bool found = false;
  while (n != ll->tail) {
    links++;
    if (n->oldspeak == NULL) {
      continue;
    } else if (eqstr(n->oldspeak, oldspeak)) {
      found = true;
      break;
    }
    n = n->next;
  }
  if (found &&
      ll->mtf) { // if mtf is enabled then it does the moves needed for it
    n->prev->next = n->next;
    n->next->prev = n->prev;
    ll->head->next->prev = n;
    n->next = ll->head->next;
    ll->head->next = n;
    n->prev = ll->head;
  }
  if (found) {
    return n;
  }
  return NULL;
}
// inserts a node at the head of the linked list
void ll_insert(LinkedList *ll, char *oldspeak, char *newspeak) {
  if (ll_lookup(ll, oldspeak) ==
      NULL) { // checks that the node doesnt already exist
    Node *n = node_create(oldspeak, newspeak);
    ll->head->next->prev = n;
    n->next = ll->head->next;
    ll->head->next = n;
    n->prev = ll->head;
  }
}
// loops through ll to print each node
void ll_print(LinkedList *ll) {
  Node *n = ll->head->next;
  while (n != ll->tail) {
    node_print(n);
    n = n->next;
  }
}
// passes the stats out
void ll_stats(uint32_t *n_seeks, uint32_t *n_links) {
  *n_seeks = seeks;
  *n_links = links;
}
