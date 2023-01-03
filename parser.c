#include "parser.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PARSER_LINE_LENGTH 1000

struct Parser {
  FILE *f;
  char current_line[MAX_PARSER_LINE_LENGTH + 1];
  uint32_t line_offset;
};
// sets up the parser
Parser *parser_create(FILE *f) {
  Parser *p = (Parser *)malloc(sizeof(Parser));
  if (f == NULL) { // checks if file pointer is null
    free(p);
    p = NULL;
    return NULL;
  }
  p->f = f;
  p->line_offset = 0;
  return p;
}
// deletes the parser
void parser_delete(Parser **p) {
  free(*p);
  *p = NULL;
}
// finds the next word in a given file
bool next_word(Parser *p, char *word) {
  if (p->line_offset == 0) { // if its the start of the line then calls fgets
    if (fgets(p->current_line, MAX_PARSER_LINE_LENGTH, p->f) ==
        NULL) { // checks if end of the line by looking for null
      return false;
    }
  }
  while (p->current_line[p->line_offset] ==
         '\n') { // loops until a non new line char is found

    if (fgets(p->current_line, MAX_PARSER_LINE_LENGTH, p->f) == NULL) {
      return false;
    }
    p->line_offset = 0; // sets the offset to start
  }
  int i = 0;         // index var for word
  bool setb = false; // flag for if a bit as been set yet
  for (p->line_offset = p->line_offset; p->line_offset < MAX_PARSER_LINE_LENGTH;
       p->line_offset++) {
    char c = p->current_line[p->line_offset]; // sets a char to current line's
                                              // char at offset
    if (isalnum(c) || (c == '-' || c == '\'')) { // checks if its a valid char
      word[i] = tolower(c); // inputs a lowcase char into word
      setb = true;          // sets the flag
      i++;
    } else if (setb) { // if flag has been set and the bit is nonvalid
      word[i] = '\0';  // sets the end of the word to an end string char
      break;           // exits the loop
    } else if (p->current_line[p->line_offset] ==
               '\n') {           // if it runs into a new line char
      p->line_offset = 0;        // sets offset to the start of the line
      return next_word(p, word); // recursively calls next_word
    }
  }
  return true;
}
