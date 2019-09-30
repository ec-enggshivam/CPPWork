#include<iostream>
using namespace std;

/*Due to multiple inheritance and all three base classes of
Platypus inheriting in turn from class Animal, you have three instances of Animal
created automatically for every instance of a Platypus, as shown in Line 38. This is
quite ridiculous as Platypus is still one animal that has inherited certain attributes from
Mammal, Bird, and Reptile. The problem in the number of instances of base Animal is
not limited to memory consumption alone. Animal has an integer member Animal::Age
(which has been kept public for explanation purposes). When you want to access
Animal::Age via an instance of Platypus, as shown in Line 42, you get a compilation
error simply because the compiler doesn’t know if you want to set
Mammal::Animal::Age or Bird::Animal::Age or Reptile::Animal::Age. It can get
even more ridiculous—if you so wanted you could set all three:
duckBilledP.Mammal::Animal::Age = 25;
duckBilledP.Bird::Animal::Age = 25;
duckBilledP.Reptile::Animal::Age = 25;
Clearly, one duck-billed platypus should have only one Age. Yet, you want class
Platypus to be a Mammal, Bird, and Reptile. The solution is in virtual inheritance.*/

class Animal{
  public:
    Animal(){
        cout<<"animal constructor"<<endl;
    }
    int age;
};

class mammal:public Animal{
};

class bird:public Animal{
};

class reptile:public Animal{
};

class platypus:public mammal, public bird, public reptile{
  public:
   platypus(){
       cout<<"platypus constructor";
   }
};

int main() {
    platypus duckbilledP;

    //The below line will give compilation error
    //duckbilledP.age = 25;

    return 0;
}
