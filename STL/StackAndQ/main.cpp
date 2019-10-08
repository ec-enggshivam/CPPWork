#include <iostream>
#include <stack>
#include <queue>

using namespace std;

typedef stack<int> stk_int;
typedef stack<string> stk_str;
typedef queue<int> q_int;
typedef queue<string> q_str;

int main(int argc, char** argv) {
	
	stk_int intStack;
	stk_str strStack;
	
	q_int intQ;
	q_str strQ;
	
	intStack.push(5);
	intStack.push(4);
	intStack.push(3);
	intStack.push(2);
	intStack.push(1);	
	
	strStack.push("shivam");
	strStack.push("vinita");
	strStack.push("Rivi");
	strStack.push("pari");
	strStack.push("mom");	
	
	while(intStack.size() > 0){			
		int num = intStack.top();
		cout<<num<<endl;
		intStack.pop();	
	}
	
	cout<<endl;
	
	while(strStack.size() > 0){			
		string str = strStack.top();
		cout<<str<<endl;
		strStack.pop();	
	}
	return 0;
}
