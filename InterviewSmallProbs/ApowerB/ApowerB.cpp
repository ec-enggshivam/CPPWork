// XORwithoutXor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <cmath>

using namespace std;


float PowAtoB(int a, int b) {
	if (b == 0) {
		return 1;
	}
	else if(b == 1){
		return a;
	}
	else {
		int b1 = abs(b);
		int a1 = 1;
		//O(log n) recursively
		/*if(b1%2 == 0){
			a1 = PowAtoB(a, b1/2) * PowAtoB(a,b1/2);
		}
		else{
			a1 = PowAtoB(a, b1/2) * PowAtoB(a,b1/2);
			a1 = a1 * a;
		}*/
		if(b1%2 == 0){
		    for(int i=0;i< b/2;i++){
				a[i] = a1*a1;
			}
		}
		
		//O(n) complexity if loop used
		/*for (int i = 1; i <= b1; i++) {
			a1 = a1 * a;
		}*/
		
		if (b < 0) {			
			return (1.0/a1);
		}
		else {
			return a1;
		}

	}
}

int main()
{
	float result;
	
	result = PowAtoB(2, 8);
	printf("\nResult: %f",result);

	result = PowAtoB(2, -8);
	printf("\nResult of -ve power: %f", result);

	result = PowAtoB(2, 0);
	printf("\nResult: %f", result);

	result = PowAtoB(-2, 8);
	printf("\nResult of -ve raised to power: %f", result);

	result = PowAtoB(0, 1);
	printf("\nResult: %f", result);

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
