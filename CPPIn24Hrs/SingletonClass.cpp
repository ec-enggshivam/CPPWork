#include<iostream>

using namespace std;


/*
 * class President discussed earlier is good, but it has a shortcoming:
 * It cannot help creation of multiple presidents via instantiation of
 * multiple objects such as: President One, Two, Three;
Individually they are non-copyable thanks to the private copy constructors,
but what you ideally need is a class President that has one, and only one,
real-world manifestation—that is, there is only one object and creation of
additional ones is prohibited.
Welcome to the concept of singleton that uses private constructors, a private assignment
operator, and a static instance member to create this (controversially) powerful
pattern.
236 LESSON 9: Classes and Objects
When the keyword static is used on a class’s data member, it
ensures that the member is shared across all instances.
When static is used on a local variable declared within the scope
of a function, it ensures that the variable retains its value between
function calls.
When static is used on a member function—a method—the
method is shared across all instances of the class.
 */
class President{
	private:
		//private default constructor(prohibits creation from outside)
		President(){};
		//private copy constructor(prohibits copy creation)
		President(const President&);
		//private assignment operator overloaded to avoid copy via assignment
		const President& operator=(const President&);

		//member data:President name
		string name;

	public:
		//controlled instantiation
    static President& GetInstance(){
      static President OnlyInstance;
      return OnlyInstance;
    }

    string get_name(){
      return name;
    }

    void set_name(string inputname){
      name = inputname;
    }
};

int main()
{
	President& OnlyPresident = President::GetInstance();
	
	//President& OnlyPresident2 = President::GetInstance();

	OnlyPresident.set_name("Abraham Lincoln");
	
	//OnlyPresident2.set_name("Abe Lin"); //the same object above gets re-initialized
	
	cout<<"the name of the president is: "<<OnlyPresident.get_name();
	
	return 0;
}

