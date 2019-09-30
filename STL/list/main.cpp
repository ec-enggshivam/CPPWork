#include <iostream>
//#include <bits\stdc++.h>
#include <list>

using namespace std;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

typedef list<int> list_int;
typedef list<string> list_str;

int main(int argc, char** argv) {
	list_str strings;
	list_str::iterator it;
	
	strings.push_back("one");
	strings.push_back("two");
	strings.push_back("three");	
	strings.push_back("four");
	strings.push_back("five");
	
	strings.push_front("0");
	
	for(it= strings.begin();it != strings.end();it++){
		cout<<*it<<" ";
	}
	
	cout<<endl<<endl;
	
	it = strings.begin();
	cout<<*it<<endl;
	it = strings.erase(it);
	
	it = strings.insert(it, "two.five");
	cout<<*it<<endl;
	
	for(it= strings.begin();it != strings.end();it++){
		cout<<*it<<" ";
	}
	
	return 0;
}
