// ArrayStackCPP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>

using namespace std;

class Stack
{
public:
	Stack();
	~Stack();

private:
	int top;
	int capacity;
	int *array;
};

Stack::Stack()
{

}

Stack::~Stack()
{
}
int main()
{

    return 0;
}

