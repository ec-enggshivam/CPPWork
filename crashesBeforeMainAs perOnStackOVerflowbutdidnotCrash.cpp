#include <iostream>

//reported as crashing before main on stackoverflow, but does not crashes before
//main
int is_array_sorted(int a[], int n)
{
if(n==1)
    return 1;
else if(a[n-1]<a[n-2])
    return 0;
is_array_sorted(a,n-1);
}

int main()
{
int a[] = {2,1,3,4} ;
int ret = is_array_sorted(a,4);

std::cout<<"sorted ->"<< ret <<std::endl;
return 0;
}
