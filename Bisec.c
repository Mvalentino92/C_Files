#include <stdio.h>
#include <math.h>
//Bisection method
int main()
{
    double a,b,x,fa,fb,fx;
    printf("Give me your two guesses: ");
    scanf("%lf,%lf",&a,&b);
    int i;
    for(i=1;i<=15;i++) {
        x = (a+b)/2;
        fx = sin(x);
        fa = sin(a);
        fb = sin(b);
        if(fa*fx <= 0) {
            b = x;
        }
        else {
            a = x;
        }
    }
    printf("The root is %f\n",x);
    return 0;
}
            
        

