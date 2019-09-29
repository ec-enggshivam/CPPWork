// BaseClassInitInheritance.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

/*
Base Class Initialization can be quite useful in class Fish wherein, by supplying a Boolean
input parameter to the constructor of Fish that initializes Fish::isFreshWaterFish,
this base class Fish can ensure that every derived class is forced to mention whether
the Fish is a freshwater one or a saltwater one as shown in Listing 10.3.
*/
#include <iostream>

using namespace std;

class Fish
{
  private:
	bool isFreshWaterFish;

  public:
    //Fish constructor
	Fish(bool isFreshWater):isFreshWaterFish(isFreshWater){}

	void swim()
	{
		if (isFreshWaterFish)
			cout << "Swims in Lake" << endl;

		else
			cout << "Swims in Sea" << endl;
	}
};

class Tuna :public Fish
{
  public:
	  Tuna() :Fish(false){}

	  void swim() {
		  cout << "tuna swims real fast"<<endl;
      }
};

class Carp :public Fish
{
  public:
	  Carp() :Fish(true){}
	  void swim() {
		  cout << "carp swims real slow"<<endl;
		  Fish::swim();
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



