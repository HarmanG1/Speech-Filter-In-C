#include "node.h"
#include <stdio.h>
#include <stdlib.h>

// self made str length function
static int strleng(char *str) {
  int x = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    x++;
  }
  return x;
}

// self made str copy function
static void strcopy(char *org, char *copy) {
  int x = 0;
  for (int i = 0; org[i] != '\0'; i++) {
    copy[i] = org[i];
    x++;
  }
  copy[x] = '\0';
}
// Sets up the node
Node *node_create(char *oldspeak, char *newspeak) {
  Node *n = (Node *)malloc(sizeof(Node));
  if (oldspeak != NULL) { // checks if oldspeak has been set
    n->oldspeak = malloc(strleng(oldspeak) + 1); // allocates memeory if it has
    strcopy(oldspeak, n->oldspeak); // calls str copy to set the node's oldspeak
  } else {
    n->oldspeak = NULL; // otherwise the node leaves it as null
  }
  if (newspeak != NULL) {
    n->newspeak = malloc(strleng(newspeak) + 1);
    strcopy(newspeak, n->newspeak);
  } else {
    n->newspeak = NULL;
  }
  n->next = NULL;
  n->prev = NULL;
  return n;
}
// deletes the node
void node_delete(Node **n) {
  if (*n != NULL) { // checks if its null before freeing
    free((*n)->oldspeak);
    free((*n)->newspeak);
    (*n)->oldspeak = NULL;
    (*n)->newspeak = NULL;
    free(*n);
    *n = NULL;
  }
}
// prints out node depending on if it has both or only 1 word set
void node_print(Node *n) {
  if (n->newspeak == NULL) {
    printf("%s\n", n->oldspeak);
  } else {
    printf("%s -> %s\n", n->oldspeak, n->newspeak);
  }
}
