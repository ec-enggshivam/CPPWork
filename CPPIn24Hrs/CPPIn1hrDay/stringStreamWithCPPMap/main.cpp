// CPP program to demonstrate use of stringstream 
// to count frequencies of words. 
#include <bits/stdc++.h> 
using namespace std; 
  
void printFrequency(string st) 
{ 
    // each word it mapped to it's frequency 
    map<string, int> FW; 
    stringstream ss(st); // Used for breaking words 
    string Word; // To store individual words 
  
    while (ss >> Word) 
        FW[Word]++; 
  
    map<string, int>::iterator m; 
    for (m = FW.begin(); m != FW.end(); m++) 
        cout << m->first << " -> "
             << m->second << "\n"; 
} 
  
// Driver code 
int main() 
{ 
    string s = "Geeks For Geeks Ide"; 
    printFrequency(s); 
    return 0; 
}
