#include <stdio.h>
#include <math.h>
#include <float.h>
//Taylor series approximation solely for exp
int main()
{
    double xguess = exp(.5);
    double x = 1.7 ;
    int fact = 1;
    int count = 0;
    double xtrue = exp(1.7);
    
    do {
        fact *= ++count;
        xguess += pow((x-0.5),count)*(exp(.5)/fact);
        printf("The value is %.20f\n",xguess);
    } while(fabs(xtrue - xguess) > FLT_EPSILON);
}
        
