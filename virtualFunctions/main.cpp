#include <iostream>

using namespace std;

//Base class
class A{
	
	public:
	virtual void display(){
		cout<<"In base class"<<endl;
	}
};

//Derived class
class B:public A{
	
	public:
	void display(){
		cout<<"In derived clas"<<endl;
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
	//show(objB);
	//ptrObjA->display2();
	
	return 0;
}
