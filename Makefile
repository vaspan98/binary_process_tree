all:	Leaf\
	Internal\
	myfind	

CC=gcc
CFLAGS=-g 

Leaf: Leaf.c
	$(CC) $(CFLAGS) Leaf.c -o Leaf

Internal: Internal.c
	$(CC) $(CFLAGS) Internal.c -o Internal -lm

myfind:$(OBJECTS)
	$(CC) $(CFLAGS) main.c -o myfind -lm

.PHONY: clean
clean:
	rm -f Leaf\
		Internal\
		myfind
