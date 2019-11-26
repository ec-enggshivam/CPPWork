#include<stdio.h>
#include<iostream>

using namespace std;

struct bitwise{
//	int:16;
//	int:16;
//	char:8;
int a;
int b;
char c;
};

struct bitwise2{
	
	char:8;
	int:16;
	int:16;
};

int main()
{
   /* int x = 20, *y, *z;

    // Assume address of x is 500 and
    // integer is 4 byte size
    y = &x;
    z = y;
    printf("y= %d\nz= %d\n", y,z);

    *y++;
    *z++;
    x++;
    printf("x = %d, y = %d, z = %d \n", x, y, z);*/

    //CASE 2:
    //======
    //bitwise bitstruct;
    //bitwise2 _bitstruct;
    cout<<"Size bitwise struct: "<<sizeof(bitwise);
		cout<<"\n\nSize bitwise2 struct with char in middle: "<<sizeof(bitwise2);
    return 0;
}
