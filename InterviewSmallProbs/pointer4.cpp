#include<stdio.h>
int main()
{
 int a = 10, b = 6;
 int *ptr;
 ptr = &b;
 printf(" %d ", a  **ptr);
 //printf(" %d ", **ptr); //Gives compilation error
 //printf(" %p ", **ptr); //Gives compilation error
 return 0;
}
