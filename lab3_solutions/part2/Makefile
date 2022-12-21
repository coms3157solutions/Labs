#
# Makefile for lab 3, part 2
#

CC = gcc
CFLAGS = -g -Wall -Wpedantic -std=c17 -I../part1

LDFLAGS = -L../part1
LDLIBS = -lmylist

revecho:
revecho.o:

.PHONY: clean
clean:
	rm -f *.o *~ a.out core revecho

.PHONY: all
all: clean revecho

