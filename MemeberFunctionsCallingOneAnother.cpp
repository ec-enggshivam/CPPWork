#include <iostream>
using namespace std;

class Date
{
	 private:
	   int day, month, year;

	 public:
		 Date (int inMonth, int inDay, int inYear)
		   : month (inMonth), day(inDay), year (inYear) {};

	 void getday() const{
     cout << day<<"/";
	 }
	 

	 void DisplayDate(){
	 	 getday();
	 	 cout << month;
		 cout<< "/" << year << endl;
	 }
};

int main ()
{
	Date holiday (12, 25, 2016); // Dec 25, 2016

	cout << "The date object is initialized to: ";
	holiday.DisplayDate ();

	return 0;
}
