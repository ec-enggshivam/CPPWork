/*
If a class Derived implements the same functions with the same return values and
	signatures as in a class Base it inherits from, it effectively overrides that method
	in class Base as shown in the following code:
	class Base
	{
	public:
	void DoSomething()
	{
	// implementation code… Does something
	}
	};
	class Derived:public Base
	{
	public:
	void DoSomething()
	{
	// implementation code… Does something else
	}
	};
	Thus, if method DoSomething() were to be invoked using an instance of Derived, then
	it would not invoke the functionality in class Base.
*/
#include <iostream>

using namespace std;

class Fish 
{
  protected:
	  bool isFreshWaterFish;
  
  public:
		//constructor for method 2
		Fish(bool _isFreshWaterFish):isFreshWaterFish(_isFreshWaterFish){}

	  void swim()
	  {
		  if (isFreshWaterFish)
		    cout << "Swims in Lake"<<endl;

		  else
			  cout << "Swims in Sea"<<endl;
	  }
};

//Method 2: passing values by intializations lists, needs a constructor to be
//implemented with parameter as the initialization is passing parameter
class Tuna:public Fish
{
  public:
	  Tuna():Fish(false){} //passing parameter value to base class from the derived class
	  
		void swim()
	  {
	    cout << "Tuna swims real fast"<<endl;
    }
};

class Carp :public Fish
{
  public:
	  Carp():Fish(true){} //passing parameter value to base class again from derived class

  void swim()
  {
    cout << "Carp swims real slow"<<endl;
  }
};


int main()
{
	Carp myLunch;
	Tuna myDinner;

	cout << "About my food: " << endl;

	cout << "My Lunch: ";
	myLunch.swim(); //cout << "Carp swims real slow"<<endl;
	cout << "My Dinner: ";
	myDinner.swim(); //cout << "Tuna swims real fast"<<endl;

	return 0;
}




