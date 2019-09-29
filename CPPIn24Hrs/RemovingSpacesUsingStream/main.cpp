// C++ program to remove spaces using stringstream 
#include<bits/stdc++.h> 
using namespace std; 
  
/* Function to remove spaces */
string removeSpaces(string str) 
{ 
    stringstream ss; 
    string temp; 
  
    /* Storing the whole string into string stream */
    ss << str; 
  
    /* Making the string empty */
    str = ""; 
  
    /* Running loop till end of stream */
    while (!ss.eof()) 
    { 
        /* extracting word by word from stream */
        ss >> temp; 
  
        /* concatenating in the string to be 
          returned*/
        str = str + temp; 
    } 
    return str; 
} 
  
/*Driver function */
int main() 
{ 
    string s = "This is a test"; 
    cout << removeSpaces(s) << endl; 
  
    s = "geeks for geeks"; 
    cout << removeSpaces(s) << endl; 
  
    s = "geeks quiz is awsome!"; 
    cout << removeSpaces(s) << endl; 
  
    s = "I   love     to     code"; 
    cout << removeSpaces(s) << endl; 
  
    return 0; 
} 
