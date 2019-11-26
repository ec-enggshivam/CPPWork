#include<stdio.h>

int main(){
 char array[5] = "Knot", *ptr, i, *ptr1;
 ptr = &array[1];
 //printf("&array[0]: %d\n", &array[0]);
 //printf("&array: %d\n", &array);
 //printf("%d\n", ptr);
 ptr1 = ptr + 3;
 *ptr1 = 101;
 //char ch = 'e';
// printf("ch: %d\n", ch);
// printf("%c\n", *ptr1);
 for(i = 0; i < 4;i++)
 printf("%c", *ptr++);
 return 0;
}
