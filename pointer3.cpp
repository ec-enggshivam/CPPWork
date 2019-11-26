#include<stdio.h>
int main()
{
 int i = 5,*ptr;
 ptr= &i;
 
 //output is 5 because typecasting can be with more asterisk
 //but not with less asterisks
 printf("\nValue of ptr =  %d ", *(int **************)ptr); 
 void *vptr;
 vptr = &ptr;
 
 //output is 5 because typecasting can be with more asterisk
 //but not with less asterisks
 printf("\nValue of i =  %d ", **(int **)vptr); 
 return 0;
}
