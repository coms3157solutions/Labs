#
# Makefile for lab 3, part 1.
#

CC = gcc
CFLAGS = -g -Wall -Wpedantic -std=c17

LDFLAGS =
LDLIBS =

AR = ar
ARFLAGS += -U

mylist-test: mylist-test.o libmylist.a
libmylist.a: libmylist.a(mylist.o)

mylist-test.o: mylist-test.c mylist.h
mylist.o: mylist.c mylist.h

.PHONY: clean
clean:
	rm -f *.o *~ a.out core libmylist.a mylist-test

.PHONY: all
all: clean libmylist.a mylist-test

