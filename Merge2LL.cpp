// LLStack.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>

using namespace std;

typedef struct StackNode {
	int data;
	struct StackNode *node;
} StackNode;

StackNode *push(StackNode *S, int data) {
	StackNode *temp = (StackNode *)malloc(sizeof(StackNode));
		if (temp) {
			temp->data = data;
			temp->node = S;
			S = temp;
			return S;
		}
		else {
			return NULL;
		}
}

int pop(StackNode **S) {
	
	if (*S) {
	  StackNode *temp = *S;
	  int data = (*S)->data;

	  *S = (*S)->node;	  
	   
	  free(temp);
		
	  return data;
	}
	else {
		return NULL;
	}
}

int main()
{
	StackNode *S = NULL;// (StackNode*)malloc(sizeof(StackNode));
	//cout <<"Enter" ;
	S = push(S, 5);
	S = push(S, 4);
	S = push(S, 3);
	S = push(S, 2);
	S = push(S, 1);
	int data = pop(&S);
	cout << data<<endl;
	data = pop(&S);
	cout << data<<endl;
    return 0;
}


