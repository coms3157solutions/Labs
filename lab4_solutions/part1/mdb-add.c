/*
 * mdb-add.c
 *
 *  This file can be compiled two different ways, using conditional compilation.
 *  That is, it can be compiled into mdb-add, which takes the mdb path as an
 *  argument, or mdb-add-cs3157, which has a hard-coded mdb path.
 *
 *  The mdb path can be hard-coded using the CONFIG_MDB_CS3157 macro, which can
 *  be defined using the -D compiler flag, e.g.:
 *
 *      gcc -c -DCONFIG_MDB_CS3157=/path/to/mdb-cs3157 mdb-add.c -o mdb-add.o
 *
 *  By default (i.e., if we don't use -D), CONFIG_MDB_CS3157 is not defined.
 *
 *  So if CONFIG_MDB_CS3157 is defined, then we assume we are compiling to
 *  mdb-add-cs3157; if CONFIG_MDB_CS3157 isn't defined, then we do not hardcode
 *  the mdb path and assume we are compiling to mdb-add.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <mylist.h>

#include "mdb.h"

static void die(const char *message)
{
    perror(message);
    exit(1);
}

static void sanitize(char *s)
{
    while (*s) {
        if (!isprint(*s))
            *s = ' ';
        s++;
    }
}

int main(int argc, char **argv)
{
    /*
     * open the database file
     */

#ifndef CONFIG_MDB_CS3157
    // if CONFIG_MDB_CS3157 is not defined, then this is compiled as mdb-add;
    // mdb path is argv[1]

    if (argc != 2) {
        fprintf(stderr, "%s\n", "usage: mdb-add <database_file>");
        exit(1);
    }

    char *filename = argv[1];
#else
    // if CONFIG_MDB_CS3157 is defined, then this is compiled as mdb-add-cs3157;
    // the CONFIG_MDB_CS3157 macro expands to hard-coded the mdb path.

    if (argc != 1) {
        fprintf(stderr, "%s\n", "usage: mdb-add-cs3157");
        exit(1);
    }

    // preprocessor hack to expand CONFIG_MDB_CS3157 into C string literal
#define xstr(s) str(s)
#define str(s) #s
    char *filename = xstr(CONFIG_MDB_CS3157);
#undef str
#undef xstr

    // set umask to 022, so that mdb-cs3157 is world-readable
    umask(S_IWGRP | S_IWOTH);
#endif

    // open for append & read, binary mode
    FILE *fp = fopen(filename, "a+b");
    if (fp == NULL)
        die(filename);

    /*
     * load the database file into a linked list
     */

    struct List list;
    initList(&list);

    // we need to call fseek() to read from the beginning
    fseek(fp, 0, SEEK_SET);

    int loaded = loadmdb(fp, &list);
    if (loaded < 0)
        die("loadmdb");

    // count the number of entries and keep a pointer to the last node
    struct Node *lastNode = list.head;
    int recNo = 0;
    while (lastNode) {
        recNo++;
        if (lastNode->next)
            lastNode = lastNode->next;
        else
            break;
    }

    /*
     * read name
     */

    struct MdbRec r;
    char line[1024];
    size_t last;

    printf("name please (will truncate to %ld chars): ", sizeof(r.name) - 1);
    if (fgets(line, sizeof(line), stdin) == NULL) {
        fprintf(stderr, "%s\n", "could not read name");
        exit(1);
    }

    // must null-terminate the string manually after strncpy().
    strncpy(r.name, line, sizeof(r.name) - 1);
    r.name[sizeof(r.name) - 1] = '\0';

    // if newline is there, remove it.
    last = strlen(r.name) - 1;
    if (r.name[last] == '\n')
        r.name[last] = '\0';

    // user might have typed more than sizeof(line) - 1 characters in line;
    // continue fgets()ing until we encounter a newline.
    while (line[strlen(line) - 1] != '\n' && fgets(line, sizeof(line), stdin))
        ;

    /*
     * read msg
     */

    printf("msg please (will truncate to %ld chars): ", sizeof(r.msg) - 1);
    if (fgets(line, sizeof(line), stdin) == NULL) {
        fprintf(stderr, "%s\n", "could not read msg");
        exit(1);
    }

    // must null-terminate the string manually after strncpy().
    strncpy(r.msg, line, sizeof(r.msg) - 1);
    r.msg[sizeof(r.msg) - 1] = '\0';

    // if newline is there, remove it.
    last = strlen(r.msg) - 1;
    if (r.msg[last] == '\n')
        r.msg[last] = '\0';

    // user might have typed more than sizeof(line) - 1 characters in line;
    // continue fgets()ing until we encounter a newline.
    while (line[strlen(line) - 1] != '\n' && fgets(line, sizeof(line), stdin))
        ;

    /*
     * add the record to the in-memory database
     */

    struct MdbRec *rec = (struct MdbRec *)malloc(sizeof(r));
    if (!rec)
        die("malloc failed");

    memcpy(rec, &r, sizeof(r));

    struct Node *newNode = addAfter(&list, lastNode, rec);
    if (newNode == NULL)
        die("addAfter() failed");

    recNo++;

    /*
     * write the name and msg to the database file
     */

    // remove non-printable chars from the strings
    sanitize(r.name);
    sanitize(r.msg);

    if (fwrite(&r, sizeof(r), 1, fp) < 1)
        die("fwrite() record");

    if (fflush(fp) != 0)
        die("fflush() file");

    /*
     * print confirmation
     */

    printf("%4d: {%s} said {%s}\n", recNo, rec->name, rec->msg);
    fflush(stdout);

    /*
     * clean up and exit
     */

    freemdb(&list);
    fclose(fp);
    return 0;
}
