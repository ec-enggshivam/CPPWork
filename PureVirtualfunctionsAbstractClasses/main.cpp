#include <iostream>

using namespace std;

//Base class
class A{
	public:
	virtual void display() = 0;
};

//Derived class
class B:public A{

	public:
	void display(){
		cout<<"In derived class B"<<endl;
	}

	/*void display2(){
		cout<<"In function display2 of derived class"<<endl;
	}*/
};

void show(A& objA){
	objA.display();
}
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int main(int argc, char** argv) {

	B objB;
	A *ptrObjA;

	ptrObjA = &objB;
	show(objB); //Passing object of type B even though the function has parameter of type A
	ptrObjA->display(); /*Calling by means of object of type A even though there is no
                            definition of display in A*/

	return 0;
}
