// ImplementingInheritance.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file


/*C++ Syntax of Derivation:
---------------------------
How would you inherit class Carp from class Fish, or in general a
class Derived from class Base?
C++ syntax for doing this would be the following:

class Base
{
// ... base class members
};

class Derived: access-specifier Base
{
// ... derived class members
};

The 'access-specifier' can be one of public (most frequently used) where a “derived
class is a base class” relationship; private or protected for a “derived class has a base
class” relationship.
Similarly, the base class is also called the super class. The class that derives from
the base, also known as the derived class, can be called the subclass.
*/


#include <iostream>

using namespace std;

class Fish 
{
  protected:
	  bool isFreshWaterFish;
  
  public:
	  void swim()
	  {
		  if (isFreshWaterFish)
		    cout << "Swims in Lake"<<endl;
		  
		  else
			cout << "Swims in Sea"<<endl;
      }


};

class Tuna:public Fish
{
  public:
	  Tuna() 
	  {
		  isFreshWaterFish = false;
	  }
};

class Carp :public Fish
{
  public:
	  Carp() {
		  isFreshWaterFish = true;
	  }
};


int main()
{
	Carp myLunch;
	Tuna myDinner;

	cout << "About my food: " << endl;

	cout << "My Lunch: ";
	myLunch.swim();
	cout << "My Dinner: ";
	myDinner.swim();

	return 0;
}




