#include "bf.h"
#include "bv.h"
#include "city.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define N_HASHES 5

struct BloomFilter {
  uint64_t salts[N_HASHES]; // Salts for five hash functions
  uint32_t n_keys;
  uint32_t n_hits;
  uint32_t n_misses;
  uint32_t n_bits_examined;
  BitVector *filter;
};

static uint64_t default_salts[] = {0x5adf08ae86d36f21, 0x419d292ea2ffd49e,
                                   0x50d8bb08de3818df, 0x272347aea4045dd5,
                                   0x7c8e16f768811a21};
// Sets up the bloom filter using provided code
BloomFilter *bf_create(uint32_t size) {
  BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
  if (bf) {
    bf->n_keys = bf->n_hits = 0;
    bf->n_misses = bf->n_bits_examined = 0;
    for (int i = 0; i < N_HASHES; i++) {
      bf->salts[i] = default_salts[i];
    }
    bf->filter = bv_create(size);
    if (bf->filter == NULL) {
      free(bf);
      bf = NULL;
    }
  }
  return bf;
}
// deletes the bloom filter by freeing its memeory
void bf_delete(BloomFilter **bf) {
  if (*bf != NULL) { // makes sure bf isnt null
    bv_delete(&(*bf)->filter);
    (*bf)->filter = NULL;
    free(*bf);
    *bf = NULL;
  }
}

uint32_t bf_size(BloomFilter *bf) {
  return bv_length(bf->filter);
} // returns the length with a call to bv

void bf_insert(BloomFilter *bf, char *oldspeak) {
  for (int i = 0; i < 5; i++) {
    uint32_t hv = hash(bf->salts[i], oldspeak) %
                  bf_size(bf);  // hashes the word to find index
    bv_set_bit(bf->filter, hv); // calls bv to set bit
  }
  bf->n_keys += 1; // increments for stats
}
// checks if bits have been set in the bit vector
bool bf_probe(BloomFilter *bf, char *oldspeak) {
  for (int i = 0; i < 5; i++) {
    uint32_t hv = hash(bf->salts[i], oldspeak) % bf_size(bf);
    uint8_t fb = bv_get_bit(bf->filter, hv);
    bf->n_bits_examined += 1;
    if (fb == 0) { // if any bit is not set that was supposed to be theen it
                   // increments misses and returns
      bf->n_misses += 1;
      return false;
    }
  }
  bf->n_hits += 1; // else it was a hit
  return true;
}
// checks for the number of set bits in bf
uint32_t bf_count(BloomFilter *bf) {
  uint32_t count = 0;
  for (uint32_t i = 0; i < bv_length(bf->filter); i++) {
    if (bv_get_bit(bf->filter, i)) {
      count += 1;
    }
  }
  return count;
}

void bf_print(BloomFilter *bf) { bv_print(bf->filter); } // calls bv to print
// passes the stats out
void bf_stats(BloomFilter *bf, uint32_t *nk, uint32_t *nh, uint32_t *nm,
              uint32_t *ne) {
  *nk = bf->n_keys;
  *nh = bf->n_hits;
  *nm = bf->n_misses;
  *ne = bf->n_bits_examined;
}
