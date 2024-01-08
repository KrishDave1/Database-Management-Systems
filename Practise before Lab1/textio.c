#include<stdio.h>
#include<stdlib.h>

int main(){
    char *filename = "first.txt";
    FILE *txtfp = fopen(filename, "w");
    char *txt = "Hello world";
    fprintf(txtfp, "%s", txt);
    fclose(txtfp);
}