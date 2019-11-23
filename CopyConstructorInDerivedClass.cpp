#include<iostream>
//#include

using namespace std;

class A{

	string nameA;

};

class B:public A{
 private:
	string name;

 public:
// 	B(const string& InputName){
// 		name = InputName;
//	 }
};



int main()
{
	B objB;
	A objA;

	objA = "Shivam ";
	return 0;
}
