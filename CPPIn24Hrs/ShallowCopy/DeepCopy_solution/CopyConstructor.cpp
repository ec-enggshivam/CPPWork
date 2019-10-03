#include<iostream>
#include <string.h>

using namespace std;


/*
The copy constructor is a special overloaded constructor that you as the programmer of
the class need to supply. The compiler guarantees invocation of the copy constructor
every time an object of the class is copied, and this includes passing an object to a function
by value.
NOTE
The declaration syntax of a copy constructor for class MyString is the following:
class MyString
{
MyString(const MyString& CopySource); // copy constructor
};
MyString::MyString(const MyString& CopySource)
{
// Copy constructor implementation code
}
Thus, a copy constructor takes an object of the same class by reference as a parameter.
This parameter is an alias of the source object and is the handle you have in writing your
custom copy code (where you would ensure a deep copy of all buffers in the source),
*/
 class MyString {
  private:
 	char* Buffer;

  public:
	// constructor
  MyString(const char* InitialInput)
  {
 	cout << "Constructor: creating new MyString" << endl;
 	if(InitialInput != NULL)
 	{
 	  Buffer = new char [strlen(InitialInput) + 1];
 	  strcpy(Buffer, InitialInput);

 	  // Display memory address pointed by local buffer
      cout << "Buffer points to: 0x" << hex;
      cout << (unsigned int*)Buffer << endl;
 	}
   else
 	Buffer = NULL;
  }

 	// Copy constructor
 	MyString(const MyString& CopySource)
 	{
 	  cout << "Copy constructor: copying from MyString" << endl;

 	if(CopySource.Buffer != NULL)
 	{	
 	  // ensure deep copy by first allocating own buffer
 	  Buffer = new char [strlen(CopySource.Buffer) + 1];

 	  // copy from the source into local buffer
 	  strcpy(Buffer, CopySource.Buffer);

 	 // Display memory address pointed by local buffer
 	 cout << "Buffer points to: 0x" << hex;
 	 cout << (unsigned int*)Buffer << endl;
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

void UseMyString(MyString Input)
{
	cout << "String buffer in MyString is " << Input.GetLength();
	cout << " characters long" << endl;
	cout << "Buffer contains: " << Input.GetString() << endl;
}

int main()
{
	MyString SayHello("Hello from String Class");

 	// Pass SayHello by value (will be copied)
 	UseMyString(SayHello);

 	return 0;
}
 
