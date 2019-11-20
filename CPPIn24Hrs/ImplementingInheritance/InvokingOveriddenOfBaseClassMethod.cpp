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
	myDinner.Fish::swim(); //Invokes base class method

	return 0;
}

