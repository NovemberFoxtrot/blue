CFLAGS=-g -Wall -Wextra -Wformat=2 -Wno-format-nonliteral -Wshadow -Wpointer-arith -Wcast-qual -Wmissing-prototypes -Wno-missing-braces -std=c99 -D_GNU_SOURCE -O2 -pedantic-errors
LDFLAGS=-lncursesw
TARGET=blue
SOURCE=blue.c

blue: blue.o
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)

blue.o:blue.c blue.h

clean:
	rm -rf *.o
	rm -rf blue
