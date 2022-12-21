/*
 * revecho.c
 */
#include <stdio.h>
#include <string.h>

#include <mylist.h>

static int cmp(const void *a, const void *b) {
    return strcmp(a, b);
}

int main(int argc, char **argv)
{
    struct List list;
    initList(&list);

    int i;
    for (i = 1; i < argc; i++)
        addFront(&list, argv[i]);

    struct Node *node = list.head;
    while (node) {
        printf("%s\n", (char *)node->data);
        node = node->next;
    }

    node = findNode(&list, "dude", &cmp);

    if (node)
        printf("\ndude found\n");
    else
        printf("\ndude not found\n");

    removeAllNodes(&list);
    return 0;
}
