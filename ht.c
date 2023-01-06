#include "ht.h"
#include "city.h"
#include "ll.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct HashTable {
  uint64_t salt;
  uint32_t size;
  uint32_t n_keys;
  uint32_t n_hits;
  uint32_t n_misses;
  uint32_t n_examined;
  bool mtf;
  LinkedList **lists;
};
// creats the hash table 
HashTable *ht_create(uint32_t size, bool mtf) {
  HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
  if (ht != NULL) {
    ht->mtf = mtf;
    ht->salt = 0x9846e4f157fe8840;
    ht->n_hits = ht->n_misses = ht->n_examined = 0;
    ht->n_keys = 0;
    ht->size = size;
    ht->lists = (LinkedList **)calloc(size, sizeof(LinkedList *));
    if (!ht->lists) {
      free(ht);
      ht = NULL;
    }
  }
  return ht;
}
// prints the hash tables underlying lists
void ht_print(HashTable *ht) {
  for (uint32_t i = 0; i < ht->size; i++) {
    if (ht->lists[i] != NULL) {
      ll_print(ht->lists[i]);
    }
  }
}
// deletes the hash table
void ht_delete(HashTable **ht) {
  if (*ht != NULL) { // checks if its null
    for (uint32_t i = 0; i < (*ht)->size;
         i++) { // loops through and clears the underlying lists
      ll_delete(&(*ht)->lists[i]);
    }
    free((*ht)->lists);
    (*ht)->lists = NULL;
  }
  free(*ht);
  *ht = NULL;
}

uint32_t ht_size(HashTable *ht) { return ht->size; } // returns the size of ht
// checks if oldspeak is in one of the lists
Node *ht_lookup(HashTable *ht, char *oldspeak) {
  uint32_t linktrackone = 0; // setting up stats values
  uint32_t holder = 0;
  ll_stats(&holder, &linktrackone);

  uint32_t hindex =
      hash(ht->salt, oldspeak) % ht_size(ht); // finds list index with hash
  if (ht->lists[hindex] == NULL) {            // if list is null then its a miss
    ht->n_misses += 1;
    return NULL;
  }
  Node *n = ll_lookup(ht->lists[hindex], oldspeak); // finds the node

  uint32_t linktracktwo = 0;
  ll_stats(&holder, &linktracktwo);
  ht->n_examined += linktracktwo - linktrackone; // updates stats
  if (n == NULL) {                               // if node is null then miss
    ht->n_misses += 1;
    return NULL;
  }
  ht->n_hits += 1; // otherwise a hit
  return n;
}
// inserts into the hashtable
void ht_insert(HashTable *ht, char *oldspeak, char *newspeak) {
  uint32_t hindex =
      hash(ht->salt, oldspeak) % ht_size(ht); // find index with hashing
  if (ht->lists[hindex] == NULL) { // if the list doesnt exist then make it
    ht->lists[hindex] = ll_create(ht->mtf);
  }
  if (ll_lookup(ht->lists[hindex], oldspeak) ==
      NULL) { // if not already in list then add to keys
    ht->n_keys += 1;
  }
  ll_insert(ht->lists[hindex], oldspeak, newspeak); // add to list
}
// counts the number of non null lists
uint32_t ht_count(HashTable *ht) {
  uint32_t counter = 0;
  for (uint32_t i = 0; i < ht->size; i++) {
    if (ht->lists[i] != NULL) {
      counter++;
    }
  }
  return counter;
}
// passes stats out
void ht_stats(HashTable *ht, uint32_t *nk, uint32_t *nh, uint32_t *nm,
              uint32_t *ne) {
  *nk = ht->n_keys;
  *nh = ht->n_hits;
  *nm = ht->n_misses;
  *ne = ht->n_examined;
}
