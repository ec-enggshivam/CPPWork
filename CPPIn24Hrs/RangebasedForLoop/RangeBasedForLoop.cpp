#include<iostream>
#include<vector>
#include<map>

using namespace std;

int main(){
	
	// Iterating over whole array 
    vector<int> v = {0, 1, 2, 3, 4, 5}; 
    int a[]={0,1,2,3, 4, 5};
    
    cout<<"This loop uses sutp and range determination through vector:"<<endl;
    
    for(auto i:v){
    	cout<<i<<' ';
	}
	
	cout<<"\nThis loop uses array:"<<endl;
	for(auto i:a){
    	cout<<i<<' ';
	}
	
	string str = "shivam";
	cout<<"\n\nThis loop uses string: "<<endl;
	for(auto i:str){
    	cout<<i<<' ';
	}
	
	cout<<"\n";
	for(char i:str){
    	cout<<i<<' ';
	}
	
	cout<<"\n\nThis loop is using actual range/type: "<<endl;	
	for(int i:{0,1,2,3,4,5}){
		cout<<i<<" ";
	}
	
	// Printing keys and values of a map 
    map <int, int> MAP({{1, 1}, {2, 2}, {3, 3}}); 
    cout<<"\n\nThis loop prints the map values: "<<endl;	
    for (auto i : MAP) 
        std::cout << '{' << i.first << ", " 
                  << i.second << "}\n"; 
		
	return 0;
	
}
