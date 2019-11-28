#include <stdio.h>
#include<iostream>

using namespace std;

int int_c[] = {1,2,3,4,5};

int main()
{
	int c =10, *cptr;
	cptr = &c;
	cout<<cptr<<endl;
	*cptr++;
	printf("c: %d\ncptr: 0x%x\n", c, cptr);

	return 0;
}
