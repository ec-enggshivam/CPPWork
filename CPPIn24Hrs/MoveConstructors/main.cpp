#include<iostream>
#include <string.h>

using namespace std;


/*
There are cases where objects are subjected to copy steps automatically, due to the nature
of the language and its needs. Consider the following:
class MyString
{
	// pick implementation from Listing 9.9
};
MyString Copy(MyString& source) // function
{
	MyString copyForReturn(source.GetString()); // create copy
	return copyForReturn; // return by value invokes copy constructor
}

int main()
{
	MyString sayHello ("Hello World of C++");
	MyString sayHelloAgain(Copy(sayHello)); // invokes 2x copy constructor
	return 0;
}

As the comment indicates, in the instantiation of sayHelloAgain, the copy constructor
was invoked twice, thus a deep copy was performed twice because of our call to function
Copy(sayHello) that returns a MyString by value. However, this value returned is very
temporary and is not available outside this expression. So, the copy constructor invoked
in good faith by the C++ compiler is a burden on performance. This impact becomes significant
if our class were to contain objects of great size.
To avoid this performance bottleneck, versions of C++ starting with C++11 feature a
move constructor in addition to a copy constructor. The syntax of a move constructor is
// move constructor
MyString(MyString&& moveSource)
{
if(moveSource.buffer != NULL)
{
buffer = moveSource.buffer; // take ownership i.e. 'move'
moveSource.buffer = NULL; // set the move source to NULL
}
}
*/
 class MyString
{
  private:
  char* Buffer;
  
  public:
  // Constructor
  MyString(const char* InitialInput)
  {
    if(InitialInput != NULL)
    {
	  Buffer = new char [strlen(InitialInput) + 1];
 	  strcpy(Buffer, InitialInput);
    }
	else
	Buffer = NULL;
  }

 	// Destructor
   ~MyString()
   {
 	 cout << "Invoking destructor, clearing up" << endl;
 	 if (Buffer != NULL)
 	 delete [] Buffer;
   }

    int GetLength()
   {
 	 return strlen(Buffer);
   }

 	const char* GetString()
   {
 	 return Buffer;
   }
};

MyString Copy(MyString& source) // function
{
	MyString copyForReturn(source.GetString()); // create copy
	return copyForReturn; // return by value invokes copy constructor
}

// move constructor
MyString(MyString&& moveSources)
{
  if(moveSource.buffer != NULL)
  {
    buffer = moveSource.buffer; // take ownership i.e. 'move'
    moveSource.buffer = NULL; // set the move source to NULL
  }
}

void UseMyString(MyString Input)
{
	cout << "String buffer in MyString is" << Input.GetLength();
 	cout << " characters long" << endl;
 
	cout << "Buffer contains: " << Input.GetString() << endl;
 	return;
}

int main()
{
  MyString sayHello ("Hello World of C++");

  MyString sayHelloAgain(Copy(sayHello)); // invokes 2x copy constructor
  										  // invokes 1x copy, 1x move constructors , if move constructor is implemented

  return 0;
}
