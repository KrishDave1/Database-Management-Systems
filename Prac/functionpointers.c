#include<stdio.h>
#include<stdlib.h>

void fun1(int x){
    printf("I am fun1\n");
    printf("%d\n", x);
}

void fun2(int a) {
    printf("I am fun2\n");
    printf("%d\n", a);
}

void callfun(void (*unknownfun) (int ), int x){
    // Here we are passing pointer to a function as a parameter to another function and it works!!
    unknownfun(x);
}

int main(){
    callfun(fun1, 10);
    callfun(fun2, 5);
}