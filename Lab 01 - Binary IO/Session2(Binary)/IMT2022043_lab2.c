#include <stdio.h>
#include <stdlib.h>

struct student
{
    int rollnum;
    char name[30];
    int age;
};

int save_num_binary(char *filename)
{
    FILE *binfp = fopen(filename, "wb");
    int num[20] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19,
                   21, 23, 25, 27, 29, 31, 33, 35, 37, 41};
    int *ptr = num;
    // for(int i = 0; i < 20; i++){
    //   fwrite((ptr + i), sizeof(int), 1, binfp);
    // fwrite uses address of the variable to write the data to the file unlike the fprintf which uses the value of the variable to write the data to the file.
    // }

    // Another Way to avoid the for loop
    fwrite(ptr, sizeof(int), 20, binfp);
    fclose(binfp);
    return 0;
}

int read_num_binary(char *filename)
{
    FILE *binfp = fopen(filename, "rb");
    int num[20];
    int *ptr = num;
    // for (int i = 0; i < 20; i++)
    // {
    //   fread((ptr + i), sizeof(int), 1, binfp);
    // }

    // Another Way to avoid the for loop
    fread(ptr, sizeof(int), 20, binfp);
    for (int i = 0; i < 20; i++)
    {
        printf("%d ", *(ptr + i));
    }
    printf("\n");
    fclose(binfp);
    return 0;
}

int save_struct_binary(char *filename)
{
    struct student s1 = {1, "Rahul", 20};
    struct student s2 = {2, "Rohit", 21};
    struct student s3 = {3, "Raj", 22};
    struct student s4 = {4, "Ravi", 23};
    struct student s5 = {5, "Rakesh", 24};
    struct student students[5] = {s1, s2, s3, s4, s5};
    struct student *ptr = students;
    FILE *binfp = fopen(filename, "wb");

    fwrite(ptr, sizeof(struct student), 5, binfp);
    fclose(binfp);
    return 0;
}

int read_struct_binary(char *filename)
{
    struct student s1;
    struct student s2;
    struct student s3;
    struct student s4;
    struct student s5;
    struct student students[5] = {s1, s2, s3, s4, s5};
    struct student *ptr = students;
    FILE *binfp = fopen(filename, "rb");

    fread(ptr, sizeof(struct student), 5, binfp);
    for (int i = 0; i < 5; i++)
    {
        printf("%d %s %d\n", (*(ptr + i)).rollnum, (*(ptr + i)).name, (*(ptr + i)).age);
    }
    fclose(binfp);
    return 0;
}
