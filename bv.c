#include "bv.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct BitVector {
  uint32_t length;
  uint64_t *vector;
};

// Contructs the bit vector
BitVector *bv_create(uint32_t length) {
  BitVector *bv = (BitVector *)malloc(sizeof(BitVector));
  if (bv == NULL) { // makes sure the allocation didnt fail
    return NULL;
  }
  bv->vector = (uint64_t *)calloc((length + 64 - 1) / 64, sizeof(uint64_t));
  bv->length = length; // sets length using the passed in value
  return bv;
}

// Deletes the bit vector
void bv_delete(BitVector **bv) {
  free((*bv)->vector); // frees the underlying parts
  (*bv)->vector = NULL;
  free(*bv);
  *bv = NULL; // frees the pointer
}

uint32_t bv_length(BitVector *bv) {
  return bv->length;
} // returns the length of bv

void bv_set_bit(BitVector *bv, uint32_t i) {
  bv->vector[i / 64] |=
      (1UL << (i % 64)); // sets the bit using the given formula for finding
                         // byte and bit pos
}

void bv_clr_bit(BitVector *bv, uint32_t i) {
  bv->vector[i / 64] &=
      ~(1UL << (i % 64)); // clears the bit using the same aformetioned formula
}

uint8_t bv_get_bit(BitVector *bv, uint32_t i) {
  return (bv->vector[i / 64] & (1UL << i % 64)) >>
         (i % 64); // retrives the bit at the position given by the formula
}

void bv_print(BitVector *bv) {
  for (uint32_t i = 0; i < (bv->length); i++) {
    printf(
        "%u",
        bv_get_bit(
            bv, i)); // calls bv get bit and uses it to print out the bit vector
  }
  printf("\n");
}
