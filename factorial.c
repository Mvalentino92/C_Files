#include <stdio.h>
//Simple Factorial script
int main()

{
    unsigned int i,n,f;
    i = 1;
    f = 1;
    
    printf("Please give me a number: ");
    scanf("%d",&n);
    
    while(i<=n) {
        f *= i;
        i++;
    }
    
    printf("factorial %u! = %u\n",n,f);
    return 0;
}
