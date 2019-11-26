#include <stdio.h>
#include<iostream>

using namespace std;

// Alignment requirements
// (typical 32 bit machine)

// char         1 byte
// short int    2 bytes
// int          4 bytes
// double       8 bytes

// structure A
typedef struct structa_tag
{
   char        c;
   short int   s;
} structa_t;

// structure B
typedef struct structb_tag
{
   short int   s;
   char        c;
   int         i;
} structb_t;

// structure C
typedef struct structc_tag
{
   char        c;
   double      d;
   int         s;
} structc_t;

// structure D
typedef struct structd_tag
{
   double      d;
   int         s;
   char        c;
} structd_t;

struct bitwise{
	int:16;
	int:16;
	char:8;
};

struct int_struct{
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
   printf("sizeof(structa_t) = %d\n", sizeof(structa_t));
   printf("sizeof(structb_t) = %d\n", sizeof(structb_t));
   printf("sizeof(structc_t) = %d\n", sizeof(structc_t));
   printf("sizeof(structd_t) = %d\n", sizeof(structd_t));
   cout<<"\nSize bitwise struct: "<<sizeof(bitwise);
   cout<<"\nSize int_struct struct, 2 ints and 1 char: "<<sizeof(int_struct);
	 cout<<"\nSize bitwise2 struct with char in middle: "<<sizeof(bitwise2);
   return 0;
}
