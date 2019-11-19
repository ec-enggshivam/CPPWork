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
  	//constructor for method 2
  	Fish(bool _isFreshWaterFish):isFreshWaterFish(_isFreshWaterFish){
		}
		
	  void swim()
	  {
		  if (isFreshWaterFish)
		    cout << "Swims in Lake"<<endl;
		  
		  else
			  cout << "Swims in Sea"<<endl;
    }
};

/*
Method 1 to pass value to base class, will work with the default constructor supplied by the compiler
class Tuna:public Fish
{
  public:
	  Tuna(){
		  isFreshWaterFish = false;
		} ////setting the variable in base class as it is protected, so accessible in derived class
};

class Carp :public Fish
{
  public:
	  Carp(){
			isFreshWaterFish = true;
		} //setting the variable in base class as it is protected, so accessible in derived class
};

*/

//Method 2: passing values by intializations lists, needs a constructor to be
//implemented with parameter as the initialization is passing parameter
class Tuna:public Fish
{
  public:
	  Tuna():Fish(false){} //passing parameter value to base class from the derived class
};

class Carp :public Fish
{
  public:
	  Carp():Fish(true){} //passing parameter value to base class again from derived class
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




