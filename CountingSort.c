#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <omp.h>
#include <string.h>

void Count_sort(int * a, int n,int threads)
{
	int i,j,count;
	int *temp = (int *)malloc(n*sizeof(int));

	#pragma omp parallel num_threads(threads)
	{
		#pragma omp for private(j,count)
		for(i = 0; i < n; i++)
		{
			count = 0;
			for(j = 0; j < n; j++)
			{
				if(a[j] < a[i]) count++;
				else if(a[j] == a[i] && j < i) count++;
			}
			temp[count] = a[i];
		}
		
		#pragma omp for
		for(i = 0; i < n; i++) a[i] = temp[i];
	}
	//memcpy(a,temp,n*sizeof(int));
	free(temp);
}

//Compare function for qsort
int qcmp(const void * a, const void * b) 
{
	return *(int *)a  - *(int *)b;
}

int main(int argc, char ** argv)
{
	int i,n,sorted,threads,*arr, *qarr;
	double start,end,ctime,qtime;

	//Get threads
	threads = atoi(argv[1]);

	n = atoi(argv[2]);
	arr = (int *)malloc(sizeof(int)*n);
	qarr = (int *)malloc(sizeof(int)*n);


	//Populate array
	srand(time(NULL));
	for(i = 0; i < n; i++) 
	{
		arr[i] = rand() % (n/3);
		qarr[i] = arr[i];
	}

	//Call Counting sort (timed)
	start = omp_get_wtime();
	Count_sort(arr,n,threads);
	end = omp_get_wtime();
	ctime = end-start;
	
	//Make sure it's sorted
	sorted = 1;
	for(i = 1; i < n; i++) 
	{
		if(arr[i] < arr[i-1])
		{
			sorted = 0;
			break;
		}
	}

	if(sorted) 
	{
		//Time and perform qsort
		start = omp_get_wtime();
		qsort(qarr,n,sizeof(int),qcmp);
		end = omp_get_wtime();
		qtime = end-start;

		//Print timing results if Counting sort was correct	
		printf("Counting Sort took %lf seconds.\n",ctime);
		printf("qsort took %lf seconds.\n",qtime);
	}
	else printf("Counting sort sorted incorrectly!");
}
