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
	
 	// vector initialized using C++11 list initialization
	std::vector<int> initVector{ 202, 2017, -1 };

	// Instantiate a vector with 10 elements (it can still grow)
	std::vector<int> tenElements (10);

	// Instantiate a vector with 10 elements, each initialized to 90
	std::vector<int> tenElemInit (10, 90);

	// Initialize vector to the contents of another
	//The below can work only with objects of like types.
	//So, you could instantiate a vecArrayCopy — a vector of integer
	//objects using another vector of integer objects. If one of them
	//were to be a vector of, say, type float, the code would not
	//compile.
	std::vector<int> copyVector (tenElemInit);

	// Vector initialized to 5 elements from another using iterators
	std::vector<int> partialCopy(tenElements.cbegin(), tenElements.cbegin() + 5);
	
	// Instantiate a vector with 4 elements, each initialized to 90
	vector <int> integers (4, 90);
	
	//	If your container needs to have very frequent insertions in the
	//middle, you should ideally choose the std::list.
	// Insert 25 at the beginning
	integers.insert (integers.begin (), 25);

	// Insert 2 numbers of value 45 at the end
	integers.insert (integers.end (), 2, 45);

	cout<<"\n\n";
	
//	Accessing elements in a vector using [] is fraught with the
//same dangers as accessing elements in an array; that is, you
//should not cross the bounds of the container. If you use the subscript
//operator ([]) to access elements in a vector at a position
//that is beyond its bounds, the result of the operation will be
//undefined (anything could happen, possibly an access violation).

	//A safer alternative is to use the at() member function:
	// gets element at position 2
	//cout < < integers.at (2);
	// the vector::at() version of the code above in
	//cout < < integers.at(index);
	//at() performs a runtime check against the size() of the
	//container and throws an exception if you cross the boundaries
	//(which you shouldn’t anyway).
	//Subscript operator ([]) is safe when used in a manner that
	//ensures bound integrity, as in the earlier example.
	for (size_t index = 0; index < integers.size (); ++index)
	{
		cout << "Element[" << index << "] = " ;
		cout << integers[index] << endl;
	}
	
	vector <int>::const_iterator element = integers.cbegin ();
 // auto element = integers.cbegin (); // auto type deduction, commented but
 				 				 	 								 		  //can be used without any errors

  cout<<"\n\n";
  
	while (element != integers.end ())
	{
		size_t index = distance (integers.cbegin (), element);

		cout << "Element at position ";
		cout << index << " is: " << *element << endl;
		// move to the next element
		++ element;
	}
	
	// Erase one element at the end
  integers.pop_back ();
  
  cout<<"\n\nAfter erase:-\n";
  for (size_t index = 0; index < integers.size (); ++index)
	{
		cout << "Element[" << index << "] = " ;
		cout << integers[index] << endl;
	}
	
	//NOTE:
	//The size of a vector is the number of elements stored in a vector. The capacity of
	//a vector is the total number of elements that can potentially be stored in the vector
	//before it reallocates memory to accommodate more elements. Therefore, a vector’s size
	//is less than or equal to its capacity.
	//You can query a vector for the number of elements by calling size():
	//cout << "Size: " << integers.size ();
	//or query it for its capacity by calling capacity():
	//cout << "Capacity: " << integers.capacity () << endl;
	//A vector can cause some amount of performance problems when it needs to frequently
	//reallocate the memory of the internal dynamic array. To a great extent, this problem
	//can be addressed by using the member function reserve(number). What reserve()
	//essentially does is increase the amount of memory allocated for the vector’s internal
	//array so as to accommodate the number of elements without needing to reallocate.
	//Depending on the type of the objects stored in the vector, reducing the number of
	//reallocations also reduces the number of times the objects are copied and saves on
	//performance.
	// instantiate a vector object that holds 5 integers of default value

   cout << "\n\nVector of integers was instantiated with " << endl;
	 cout << "Size: " << integers.size ();
	 cout << ", Capacity: " << integers.capacity () << endl;

	 // Inserting a 6th element in to the vector
	 integers.push_back (666);

	 cout << "After inserting an additional element... " << endl;
	 cout << "Size: " << integers.size ();
	 cout << ", Capacity: " << integers.capacity () << endl;

	 // Inserting another element
	integers.push_back (777);

	 cout << "After inserting yet another element... " << endl;
	 cout << "Size: " << integers.size ();
	 cout << ", Capacity: " << integers.capacity () << endl;

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
