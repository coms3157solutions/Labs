CC = gcc
CFLAGS = -g -Wall -Wpedantic -std=c17
LDFLAGS =
LDLIBS =

twecho: twecho.o main.o
twecho.o: twecho.c twecho.h
main.o: main.c twecho.h

.PHONY: clean
clean:
	rm -f *.o a.out twecho

.PHONY: all
all: clean main
