CFLAGS=-g -Wall -Wextra -Wformat=2 -Wno-format-nonliteral -Wshadow -Wpointer-arith -Wcast-qual -Wmissing-prototypes -Wno-missing-braces -std=c99 -D_GNU_SOURCE -O2 -pedantic-errors
LIBS=-lncursesw

all: blue

blue: blue.o

blue.o:blue.c

clean:
	rm -rf *.o
