# Implementation of a Speech filter in C

The files all come together to implement a hashtable and bloomfilter to regulate the speech that is passed into it. Inspired by my second year CS coursework. 

# Building

Run the following to build the 'banhammer' executable
```
make
```

or 

```
make all
```

Run the following to delete executables and objects

```
make clean
```

# Running banhammer
```
./banhammer
```

## Files

### banhammer.c

Banhammer uses `badspeak.txt` and `goodspeak.txt` to regulate speech that is passed in from stdin 

- <code>-t size</code>: specifies that the hash table will have size entries (the default will be 10000).
- <code>-f size</code>: specifies that the Bloom filter will have size entries (the default will be 2^19).
- <code>-m</code>: will enable the move-to-front rule. By default, the move-to-front rule is disabled.
- <code>-s</code>: enables verbose output.
- <code>-h</code>: displays program synopsis and usage.

### bv.c,h

Contains the source and header files for the Bitvector implementation

### bf.c,h

Contains the source and header files for the Bloomfilter implementation 

### node.c,h

Contains the source and header file for the node implementation

### ll.c,h

Contains the sorce and header file for the Linkedlist implementation

### ht.c,h

Contains the source and header file for the Hashtable implementation

### parser.c,h

Contains the source and header file for the parser implementation

### Makefile

Makefile contains the commands to complie, clean, and format the files

### messages.h

Contains the given messages for users who commit bad or oldspeak
