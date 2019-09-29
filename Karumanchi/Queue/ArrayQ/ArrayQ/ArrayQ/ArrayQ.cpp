// ArrayQ.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<malloc.h>
#include<iostream>

using namespace std;

typedef struct ArrayQ {
	int front, rear;
	int capacity;
	int *array;
} ArrayQ;

bool IsQFull(ArrayQ *Q) {
	return((Q->rear + 1)/Q->capacity) == Q->front);
}

ArrayQ *EnQ(ArrayQ *Q, int data){
	if (Q->rear == Q->front) {
		
	}
	else if (!IsQFull(Q)) {
		Q->array[++(Q->rear)] = data;
	}
	else
	{
		cout << "Q full";
	}
}

int main()
{
	ArrayQ *Q = (ArrayQ *)malloc(sizeof(ArrayQ));

	if (Q) {
		EnQ(Q, 5);
	}
    return 0;
}

