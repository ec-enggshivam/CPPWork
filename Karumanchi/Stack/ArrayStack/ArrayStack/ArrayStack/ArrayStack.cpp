// ArrayStack.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "malloc.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

typedef struct ArrayStack {
	int top;
	int capacity;
	int *array;
} ArrayStack;

ArrayStack *CreateStack(int capacity) {
	ArrayStack *stack = (ArrayStack *)malloc(sizeof(ArrayStack));

	if (!stack) {
		return NULL;
	}

	stack->capacity = capacity;
	stack->array = (int *)malloc(sizeof(int)*capacity);
	stack->top = -1;
	return stack;
}

bool IsStackFull(ArrayStack *S) {
	return (S->top == (S->capacity - 1));
}

void push(ArrayStack *S, int val) {
	if (!IsStackFull(S)) 
		S->array[++S->top] = val;	
}

bool IsStackEmpty(ArrayStack *S) {
	return (S->top == -1);
}

int pop(ArrayStack *S) {
	if (!IsStackEmpty(S)) {
		return S->array[S->top--];
	}
	else {
		return 0;
	}
}

int main()
{
	int size;
	cout << "Enter the size of stack" << endl;
	cin >> size;
	ArrayStack *S = CreateStack(size);
	push(S, 5);
	push(S, 4);
	size = pop(S);
	cout << size<<endl;
    return 0;
}

