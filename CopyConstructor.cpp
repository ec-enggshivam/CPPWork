#include<iostream>
#include<string.h>

using namespace std;

class A{
	string nameA;

public:
	A(){
	}

	A(string _name){
		nameA = _name;
 	}

	A(const A& InputName){
 		this->nameA = InputName.nameA;
	 }

	const string GetString(){
		return nameA;
 }
};

class B:public A{
 private:
	string name;

 public:
 	B(){

 	}

 	B(string _name){
		name = _name;
 	}

 	B(const B& InputName){
 		this->name = InputName.name;
	}

	int GetLength(){
  	//return strlen(name);
	}

	const string GetString(){
		return name;
	}
};

void useMyString(A CopyStr){
	cout << "String buffer in MyString is " << CopyStr.GetString();
	//cout << CopyStr.GetLength()<<" characters long" << endl;
}

int main()
{
	B objB("Shivam");
	A objA("Shukla");

  //B *objBptr = &objB;

	useMyString(objB);
	return 0;
}
