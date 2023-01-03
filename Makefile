CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic -Ofast -gdwarf-4

all: banhammer

banhammer: banhammer.o bv.o bf.o node.o ll.o ht.o parser.o city.o
	$(CC) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o 

spotless: clean
	rm -f banhammer

format:
	clang-format -i -style=file *.[ch]
