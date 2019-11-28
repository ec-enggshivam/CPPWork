#include <bits/stdc++.h> 
using namespace std; 
  
void printRepeating(int arr[], int size)  
{  
    int Xor = arr[0]; /* Will hold Xor of all elements */
    int set_bit_no; /* Will have only single set bit of Xor */
    int i;  
    int n = size - 2;  
    int x = 0, y = 0;  
          
    /* Get the Xor of all elements in arr[] and {1, 2 .. n} */
    for(i = 1; i < size; i++)  
        Xor ^= arr[i];  
    for(i = 1; i <= n; i++)  
        Xor ^= i;  
      
    /* Get the rightmost set bit in set_bit_no */
    set_bit_no = Xor & ~(Xor-1);  
      
    /* Now divide elements in two sets by comparing rightmost set  
    bit of Xor with bit at same position in each element. */
    for(i = 0; i < size; i++)  
    {  
        if(arr[i] & set_bit_no)  
        x = x ^ arr[i]; /*Xor of first set in arr[] */
        else
        y = y ^ arr[i]; /*Xor of second set in arr[] */
    }  
    for(i = 1; i <= n; i++)  
    {  
        if(i & set_bit_no)  
        x = x ^ i; /*Xor of first set in arr[] and {1, 2, ...n }*/
        else
        y = y ^ i; /*Xor of second set in arr[] and {1, 2, ...n } */
    }  
          
    cout<<"The two repeating elements are "<<y<<" "<<x;  
}  
  
// Driver code 
int main()  
{  
    int arr[] = {4, 2, 4, 5, 2, 3, 1};  
    int arr_size = sizeof(arr)/sizeof(arr[0]);  
    printRepeating(arr, arr_size);  
    return 0;  
}  