#include "bf.h"
#include "bv.h"
#include "city.h"
#include "ht.h"
#include "ll.h"
#include "messages.h"
#include "node.h"
#include "parser.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define OPTIONS "t:f:msh"

// self made string copy function
static void stringcopy(char *org, char *copy) {
  int x = 0;
  for (int i = 0; org[i] != '\0'; i++) {
    copy[i] = org[i];
    x++;
  }
  copy[x] = '\0';
}

int main(int argc, char **argv) {
  // sets up vars and their default values
  uint32_t tsize = 10000;
  uint32_t fsize = 1 << 19;
  bool mtf = false;
  bool pstats = false;

  int opt = 0;
  // Parses the command line args
  while (((opt = getopt(argc, argv, OPTIONS)) != -1)) {
    // Swtich statement based off the command line arg passed
    switch (opt) {
    case 't':
      // sets new value
      tsize = strtoul(optarg, NULL, 10);
      if (tsize <= 0) { // checking lower bound
        fprintf(stderr, "./banhammer: Invalid hash table size.\n");
        return 1;
      }
      break;
    case 'f':
      fsize = strtoul(optarg, NULL, 10);
      if (fsize <= 0) { // checks lower bound
        fprintf(stderr, "./banhammer: Invalid bloom filter table size.\n");
        return 1;
      }
      break;
    case 'm':
      mtf = true; // sets flag for mtf
      break;
    case 's':
      pstats = true; // sets flag for stats
      break;
    case 'h':
      // help message
      fprintf(stderr, "%s",
              "Usage: ./banhammer [options]\n"
              "  ./banhammer will read in words from stdin, identify any "
              "badspeak or old speak and output an\n"
              "  appropriate punishment message. The badspeak and oldspeak "
              "(with the newspeak translation)\n"
              "  that caused the punishment will be printed after the message. "
              "If statistics are enabled\n"
              "  punishment messages are suppressed and only statistics will "
              "be printed.\n"
              "    -t <ht_size>: Hash table size set to <ht_size>. (default: "
              "10000)\n"
              "    -f <bf_size>: Bloom filter size set to <bf_size>. (default "
              "2^19)\n"
              "    -s          : Enables the printing of statistics.\n"
              "    -m          : Enables move-to-front rule.\n"
              "    -h          : Display program synopsis and usage.\n");
      return 0;
    case '?':
      // handles unknown messages
      fprintf(stderr, "./banhammer: invalid option -- '%c'\n %s", opt,
              "Usage: ./banhammer [options]\n"
              "  ./banhammer will read in words from stdin, identify any "
              "badspeak or old speak and output an\n"
              "  appropriate punishment message. The badspeak and oldspeak "
              "(with the newspeak translation)\n"
              "  that caused the punishment will be printed after the message. "
              "If statistics are enabled\n"
              "  punishment messages are suppressed and only statistics will "
              "be printed.\n"
              "    -t <ht_size>: Hash table size set to <ht_size>. (default: "
              "10000)\n"
              "    -f <bf_size>: Bloom filter size set to <bf_size>. (default "
              "2^19)\n"
              "    -s          : Enables the printing of statistics.\n"
              "    -m          : Enables move-to-front rule.\n"
              "    -h          : Display program synopsis and usage.\n");
      return 1;
    }
  }
  // makes the needed structs
  BloomFilter *bf = bf_create(fsize);
  HashTable *ht = ht_create(tsize, mtf);
  // sets up file pointer for badspeak
  FILE *fbad;
  fbad = fopen("badspeak.txt", "r");
  if (fbad == NULL) {
    fprintf(stderr, "File opening failed!\n");
    return 1;
  }
  // sets up file pointer for newspeak
  FILE *fnew;
  fnew = fopen("newspeak.txt", "r");
  if (fnew == NULL) {
    fprintf(stderr, "File opening failed!\n");
    return 1;
  }
  // sets up file parser for badspeak and oldspeak
  Parser *pbad = parser_create(fbad);
  Parser *pnew = parser_create(fnew);

  char bad[MAX_PARSER_LINE_LENGTH + 1];
  while (next_word(pbad, bad)) { // parses through badspeak and adds to structs
    bf_insert(bf, bad);
    ht_insert(ht, bad, NULL);
  }

  char new[MAX_PARSER_LINE_LENGTH + 1];
  char oldlast[MAX_PARSER_LINE_LENGTH + 1];
  int i = 0;
  while (next_word(pnew, new)) { // parses through newspeak and adds to structs
    if (i % 2 == 0) {            // if its oldspeak then adds to bf
      bf_insert(bf, new);
      stringcopy(new, oldlast); // makes a copy
    } else {
      ht_insert(ht, oldlast,
                new); // for new speak it passes in along with the copy to ht
    }
    i++;
  }
  // sets up linked lists for user bad and old speak
  LinkedList *llbad = ll_create(mtf);
  LinkedList *llnew = ll_create(mtf);
  bool foundbad = false; // sets up flags for which is found
  bool foundold = false;
  // sets up file pointer for standard input
  FILE *fin = stdin;
  Parser *pin = parser_create(fin);
  char in[MAX_PARSER_LINE_LENGTH + 1];
  while (next_word(
      pin,
      in)) { // parses the standard input and checks if its bad or old speak
    if (bf_probe(bf, in)) {
      Node *n = ht_lookup(ht, in);
      if (n != NULL) {
        if (n->newspeak == NULL) {
          foundbad = true;
          ll_insert(llbad, n->oldspeak, NULL);
        } else {
          foundold = true;
          ll_insert(llnew, n->oldspeak, n->newspeak);
        }
      }
    }
  }
  // if the stats flag was not set then print out the given messages
  if (!pstats) {
    if (foundbad && foundold) { // checks which flags have been set to see what
                                // message to print out
      printf("%s", mixspeak_message);
      ll_print(llbad);
      ll_print(llnew);
    } else if (foundbad) {
      printf("%s", badspeak_message);
      ll_print(llbad);
    } else if (foundold) {
      printf("%s", goodspeak_message);
      ll_print(llnew);
    }
  } else if (pstats) { // if stats flag is set then it outputs the stats
    uint32_t hnk, hnh, hnm, hne;
    ht_stats(ht, &hnk, &hnh, &hnm, &hne);
    uint32_t bnk, bnh, bnm, bne;
    bf_stats(bf, &bnk, &bnh, &bnm, &bne);

    float bepm;
    if (bnm == 0) { // checks for any divides by zero
      bepm = 0;
    } else {
      bepm = (bne - N_HASHES * bnh) / (float)bnm; // casts to float
    }
    float fp;
    if (bnh == 0) {
      fp = 0;
    } else {
      fp = hnm / (float)bnh;
    }

    float bfl;
    if (bf_size(bf) == 0) {
      bfl = 0;
    } else {
      bfl = bf_count(bf) / (float)bf_size(bf);
    }
    float asl;
    if ((hnh + hnm) == 0) {
      asl = 0;
    } else {
      asl = hne / (float)(hnh + hnm);
    }
    // prints out stats with given order and heading
    printf("ht keys: %u\nht hits: %u\nht misses: %u\nht probes: %u\n", hnk, hnh,
           hnm, hne);
    printf("bf keys: %u\nbf hits: %u\nbf misses: %u\n", bnk, bnh, bnm);
    printf("bf bits examined: %u\n", bne);
    printf("Bits examined per miss: %f\n", bepm);
    printf("False positives: %f\n", fp);
    printf("Average seek length: %f\n", asl);
    printf("Bloom filter load: %f\n", bfl);
  }
  // clears and frees the structs and file pointers
  ll_delete(&llbad);
  ll_delete(&llnew);
  parser_delete(&pbad);
  parser_delete(&pnew);
  parser_delete(&pin);
  fclose(fbad);
  fclose(fnew);
  bf_delete(&bf);
  ht_delete(&ht);

  return 0;
}
