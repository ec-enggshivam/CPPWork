/*
	Derived Class Hiding Base Class’s Methods
	Overriding can take an extreme form where Tuna::Swim() can potentially hide all overloaded
	versions of Fish::Swim() available, even causing compilation failure when the
	overloaded ones are used (hence, called hidden), as
*/

#include <iostream>

using namespace std;

class Fish
{
  public:
	  void swim()
	  {
		  cout << "fish swims..."<<endl;
	  }
	  
	  void swim(bool isFreshWaterFish)
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
  	using Fish::swim; //will not work without this
  	
		void swim()
	  {
	    cout << "Tuna swims real fast"<<endl;
    }
};


int main()
{
	Tuna myDinner;

	cout << "About my food: " << endl;

	cout << "My Dinner: ";
	myDinner.swim(true); //-> results in compilation failure without the keyword using in derived class Tuna
											 //as base class method is hidden by the derived class
	//note that swim is present in both base and derived class, so above does not
	//works and below works
	myDinner.swim(); //invokes the derived class Tuna swim
	//To call the base class swim use scopresolution operator
	myDinner.Fish::swim(false); //OR put using keyowrd in derived class as shown above

	return 0;
}

