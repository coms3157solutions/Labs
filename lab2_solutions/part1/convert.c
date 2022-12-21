#include <stdio.h>  // for printf()
#include <stdlib.h> // for atoi()

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Error: no argument specified.\n");
        return 1;
    }

    int x = atoi(argv[1]); // Parse the first argument as an int

    printf("signed dec:   %d\n", x);
    printf("unsigned dec: %u\n", x);
    printf("hex:          %x\n", x);

    printf("binary:       ");
    for (int i = sizeof(int) * 8 - 1; i >= 0; i--) {
        printf("%d", (x >> i) & 1);
        if (i % 4 == 0)
            printf(" ");
    }
    printf("\n");

    return 0;
}
