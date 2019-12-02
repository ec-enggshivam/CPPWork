#include<stdio.h>


int main()
{
	const static int x = 10;
	int *xptr = (int *)&x;
	*xptr = 11;
	printf("val of x: %d", x);
}
