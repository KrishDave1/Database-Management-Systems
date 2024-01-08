#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *filename = "first.bin";
    FILE *binfp = fopen(filename, "wb");
    int num = 10;
    fwrite(&num, sizeof(int), 1, binfp);
    fclose(binfp);
}