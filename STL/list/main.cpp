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

/*
// GeeksForgeeks: C++ code to demonstrate the working of 
// insert() function 
  
#include <iostream> 
#include <list> // for list operations 
using namespace std; 
  
int main() 
{ 
    // declaring list 
    list<int> list1; 
  
    // using assign() to insert multiple numbers 
    // creates 3 occurrences of "2" 
    list1.assign(3, 2); 
  
    // initializing list iterator to beginning 
    list<int>::iterator it = list1.begin(); 
  
    // iterator to point to 3rd position 
    advance(it, 2); 
  
    // using insert to insert 1 element at the 3rd position 
    // inserts 5 at 3rd position 
    list1.insert(it, 5); 
  
    // Printing the new list 
    cout << "The list after inserting"
         << " 1 element using insert() is : "; 
    for (list<int>::iterator i = list1.begin(); 
         i != list1.end(); 
         i++) 
        cout << *i << " "; 
  
    cout << endl; 
  
    // using insert to insert 
    // 2 element at the 4th position 
    // inserts 2 occurrences 
    // of 7 at 4th position 
    list1.insert(it, 2, 7); 
  
    // Printing the new list 
    cout << "The list after inserting"
         << " multiple elements "
         << "using insert() is : "; 
  
    for (list<int>::iterator i = list1.begin(); 
         i != list1.end(); 
         i++) 
        cout << *i << " "; 
  
    cout << endl; 
}
*/ 
/*Output:
The list after inserting 1 element using insert() is : 2 2 5 2 
The list after inserting multiple elements using insert() is : 2 2 5 7 7 */

