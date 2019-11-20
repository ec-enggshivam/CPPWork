void main
{
    int i;
    int *x1;
    int *x2;
 x1=&i;

x2= func(); //call function to create stack
if(x2>x1) 
    printf("Stack Grows low to high address - upwards");
else
    printf("Downwards");
}

int *func()
{
int x;
return &x; //return address of x
} 
this is logic for stack gwroing upward and downward
just compare address x1 and x2