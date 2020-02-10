#include<iostream>
#include<string.h>

using namespace std;

unsigned int mystringLen(char *str)
{
	unsigned int length;
	while(*str != '\0')
	{
		++length;
		++str;
	}
	
	return length;
}

void RemoveLeadingAndTrailingSpaces(char *str)
{
	unsigned int len = mystringLen(str);
		
	cout<<"\nLength of the string is: "<<len<<endl;
	len = len - 1;
	
	while(str[len] == ' '){
		len -= 1;
	}
	
	char *pstr = new char[len];	
	
	str[len+1] = '\0';
		
	while(*str == ' '){
		++str;
	}	
	
//	while(*str != '\0'){
//		*pstr = *str;		
//		++pstr;
//		++str;
//	}
	
	unsigned int i = 0;	
	
		
//	while(i< len){
//	  cout<<str[i]<<"\n";
//	  ++i;			
//	}	
}


int main()
{
	char str[500];	
	char *str_result;
	cout<<"Enter a string to remove spaces: ";
	gets(str);	
	
	RemoveLeadingAndTrailingSpaces(str);
	
	return 0;
}
