#include <stdio.h>
#include <math.h>

//Serial GPU sieve.
__global__ void sieve(int *nums, int n)
{
    int i,j;
    for(i = 2; i <= (int)sqrt((double)n); i++)
    {
        if(nums[i] > 0)
        {
            for(j = i+i; j <= n; j += i) nums[j] = 0;
        }
    }
}

//Serial CPU sieve.
void cpusieve(int *nums, int n)
{
    int i,j;
    for(i = 2; i <= (int)sqrt((double)n); i++)
    {
        if(nums[i] > 0)
        {
            for(j = i+i; j <= n; j += i) nums[j] = 0;
        }
    }
}

//Parallel GPU sieve.
__global__ void psieve(int *nums, int s, int n)
{
    int i;
    int bid = blockIdx.x + s; //current number
    int bidStart = bid + bid; //Where to start if prime.
 
    //If this number is prime (not marked composite)
    if(nums[bid] > 0)
    {
        int span = (n-bidStart)/bid; //The span of multiples to mark.
        int thit = span/blockDim.x + 1; //How many each thread will hit.
        int tid = bidStart + threadIdx.x*(thit+1)*bid; //The starting index of each thread.
 
        int bound = tid + bid*thit > n ? n : tid + bid*thit; //Last thread, might overshoot. Go til n.
        for(i = tid; i <= bound; i += bid) //Increment by number.
        {
            nums[i] = 0;    
        }
    }
}
int main(int argc, char **argv)
{
    int i, n, c, arg, nsqrt, blocks, s, t, *numbers, *dev_numbers,*params, *dev_params;
    clock_t start, end;
    n = atoi(argv[1]);
    nsqrt = (int)sqrt((double)n);
    arg = atoi(argv[2]);
 
    //Start the clock, and initialize array of numbers from 1 to n
    start = clock();
    numbers = (int *)malloc(sizeof(int)*(n+1));
    for(i = 0; i <= n; i++) numbers[i] = i;
 
    //Do serial CPU
    if(arg == 0)
    {
        cpusieve(numbers,n);
        end = clock();
    }
 
    //cudaMalloc and Memcpy required vars for both kernals.
    else if(arg == 1 || arg == 2)
    {
        cudaMalloc((void**)&dev_numbers,sizeof(int)*(n+1));
        cudaMemcpy(dev_numbers,numbers,sizeof(int)*(n+1),cudaMemcpyHostToDevice);
     
        //If it's the serial kernel, run it.
        if(arg == 2) sieve<<<1,1>>>(dev_numbers,n);
     
        //If its parallel, init other needed vars, and run it.
        else
        {
            s = 2;
            t = 2;
  
            //Keep pushing through completed stretches of numbers.
            while(s <= nsqrt)
            {
                blocks = t < nsqrt ? t - s + 1 : nsqrt - s + 1;
                psieve<<<blocks,128>>>(dev_numbers,s,n);
                s = t + 1;
                t *= t;
            }
        }
     
        //Copy to host, and end clock for kernel funcs.
        cudaMemcpy(numbers,dev_numbers,sizeof(int)*(n+1),cudaMemcpyDeviceToHost);
        end = clock();
    }
 
    //Print the numbers
    if(argc > 3)
    {
        c = 1;
        for(i = 0 ; i <= n; i++) 
        {
            if(c % 25 == 0) 
            {
                printf("\n");
                c = 1;
            }
            if(numbers[i] > 1) 
            {
                printf("%d ",numbers[i]);
                c++;
            }
        }
    }
    printf("\nTime: %lf\n",((double)(end-start))/CLOCKS_PER_SEC);
}
