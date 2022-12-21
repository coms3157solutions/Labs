CC = gcc

# Support externally overriding CFLAGS
CFLAGS ?= -g -Wall -Wpedantic -std=c17
CFLAGS += -I/home/j-hui/cs3157-pub/include

LDFLAGS = -L/home/j-hui/cs3157-pub/lib
LDLIBS = -lmylist

.PHONY: default
default: mdb-add mdb-lookup

mdb-lookup: mdb.o

mdb-add: mdb.o

mdb-add.o: mdb.h

mdb-lookup.o: mdb.h

mdb.o: mdb.h

.PHONY: clean
clean:
	rm -f *.o a.out core mdb-add mdb-lookup

.PHONY: all
all: clean default
