#include<stdio.h>
int main(){
 char *ptr = "void pointer";
 void *vptr;
 vptr = &ptr;
 printf("%s" , *(char **)vptr);
 return 0;
}
