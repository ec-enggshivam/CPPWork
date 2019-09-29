#include <iostream>
//#include <bits\stdc++.h>
#include <vector>

using namespace std;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

typedef vector<int> vector_int;
typedef vector<string> vector_str;

int main(int argc, char** argv) {
	
	vector_str vectstring;
	
	vectstring.push_back("one");
	vectstring.push_back("2");
	vectstring.push_back("3");
	
	vector_str::iterator it = vectstring.begin();
	
	cout<<*it<<endl;
	cout<<"Size is: "<<vectstring.size()<<endl; //size is: 3
	cout<<"Capacity is : "<<vectstring.capacity()<<endl; //capacity is 4, why ?
	it++;
	cout<<*it<<endl;
	return 0;
}



/*
//============================================================================
// Name        : Vectors.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <string> 

using namespace std;

int main() {

	vector<string> strings;

	strings.push_back("one");
	strings.push_back("two");
	strings.push_back("three");

	cout << "For loop: " << endl;
	for(int i=0; i<strings.size(); i++) {
		cout << strings[i] << endl;
	}
	cout << endl;

	cout << "Iterator loop: " << endl;
	for(auto it = strings.begin(); it != strings.end(); ++it) {
		cout << *it << endl;
	}
	cout << endl;

	cout << "Single item." << endl;
	vector<string>::iterator it = strings.begin();
	it += 2;

	cout << *it << endl;

	return 0;
}
*/
