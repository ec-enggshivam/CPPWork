#include<stdio.h>
#include<iostream>

using namespace std;

int main()
{
    int x = 20, *y, *z;

    // Assume address of x is 500 and
    // integer is 4 byte size
    y = &x;
    z = y;
    printf("y= %d\nz= %d\n", y,z);
    
    *y++;
    *z++;
    x++;
    printf("x = %d, y = %d, z = %d \n", x, y, z);
    return 0;
}
