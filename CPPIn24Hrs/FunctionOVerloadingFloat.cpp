#include<iostream>

using namespace std;

void test(float s=0.0,float t=0.0)
{
    cout << "Function with float called ";
}

void test(int s,int t)
{
    cout << "Function with int called ";
}

int main()
{
    test(); // Added suffix "f" to both values to
                     // tell compiler, it's a float value
    return 0;
}
