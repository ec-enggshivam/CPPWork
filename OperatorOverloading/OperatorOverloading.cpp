#include <iostream>
#include <string>

using namespace std;

class test {
    string name;
    int id;

  public:
    test(string name, int id) : name(name), id(id) {
      cout << "Constructor called\n";
    }

    void print() {
      cout << name << ":" << id;
    }
};

int main(int argc, char **argv) {

  test test1("rivi", 10);

  test test2("vini", 25);

  test2 = test1; //shallow copying as no copy constructor is there

  test2.print();

  return 0;
}
