#include <stdio.h>
#include <math.h>
#include <stdlib.h>
//Question from Project Euler that I solved.
int main()
{
    int total = 0;
    int i,Num,x,Numhold;
    for(i=1;i<=1000;i++) {
        Num = i;
        while(Num != 89 & Num != 1) {
            Numhold = 0;
            while(Num>=10) {
                Numhold += pow((Num % 10),2);
                Num /= 10;
            }
            Numhold += pow(Num,2);
            Num = Numhold;
        }
        if(Num==89) {
            total+=1;
        }
    }
    printf("Total: %d\n",total);
    return 0;
}

                
        
