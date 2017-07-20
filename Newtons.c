#include <stdio.h>
#include <math.h>
#include <float.h>
#define fun(X) pow((X),2) - 2*X
//Root finding numerically with the Newton Method
int main()
{
    double tol = .01,left,right,xguess,x,deriv,x0;  //Tol is going to help us get the Numerical Derivative
    int iter,count = 0;
    
    printf("What is you\'re guess for the function?: ");
    scanf("%lf",&xguess);
    x = xguess;
    x0 = x + FLT_EPSILON*2;
    while(fabs(x0-x) > FLT_EPSILON) {       //Keep going until it is accurate
        deriv = ((fun(x+tol))-(fun(x-tol)))/(tol*2);   //Get the difference between the two surrounding
        x0 = x;                                        //values of the value we want and divide by timestep
        x = x - ((fun(x))/deriv);
        count++;
    }
    printf("The root is %f and it took %d iterations\n",x,count);
    return 0;
}
    
    
