#include <iostream>
#include <bits\stdc++.h>

using namespace std;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

typedef map<int, string> map_int;

int main(int argc, char** argv) {
	
	map_int hashtable;
	
	hashtable[10]="Shivam";
	hashtable[11]="S2";
	hashtable[12]="S3";
	hashtable[13]="S4";
	
	map_int::iterator it = hashtable.begin();
	
	
	cout<<it->first<<" "<<it->second<<endl;
	
    for(;it != hashtable.end(); it++) {
		pair<int, string> new_hashtable = *it;

		cout << new_hashtable.first << ": " << new_hashtable.second << endl;
	}
	
	return 0;
}



//============================================================================
// Name        :
// Author      : John Purcell
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
/*
#include <iostream>
#include <map>
#include <string>

using namespace std;

int main() {

	map<string, int> ages;

	ages["Mike"] = 40;
	ages["Raj"] = 20;
	ages["Vicky"] = 30;

	ages["Mike"] = 70;

	ages.insert(make_pair("Peter", 100));

	cout << ages["Raj"] << endl;

	if(ages.find("Vicky") != ages.end()) {
		cout << "Found Vicky" << endl;
	}
	else {
		cout << "Key not found." << endl;
	}

	for(map<string, int>::iterator it = ages.begin(); it != ages.end(); it++) {
		pair<string, int> age = *it;

		cout << age.first << ": " << age.second << endl;
	}

	for(map<string, int>::iterator it = ages.begin(); it != ages.end(); it++) {
			cout << it->first << ": " << it->second << endl;
		}


	return 0;
}
*/
