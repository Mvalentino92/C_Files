#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>
#include <limits.h>

unsigned short work[3];
#pragma omp threadprivate(work)

int main(int argc, char ** argv)
{
	int i, j, hits,throws,threads,threadnum;
	double x,y,distance,pi,start,end;
	
	//Get thread number, and initialize the work matrix.
	threads = atoi(argv[1]);

	//Get throws
	throws = atoi(argv[2]);

	//Throw darts
	hits = 0;
	start = omp_get_wtime();
	#pragma omp parallel num_threads(threads)
	{	
		work[0] = (unsigned short)time(NULL);
		work[1] = (unsigned short)time(NULL);
		work[2] = (unsigned short)omp_get_thread_num();
		#pragma omp for private(x,y,distance) reduction(+:hits)
		for(i = 0; i < throws; i++)
		{
			x = (erand48(work) - 0.5)*2;
			y = (erand48(work) - 0.5)*2;
			distance = x*x + y*y;

			if(distance <= 1) hits++;
		}
	}

	pi = 4.0*hits/throws;
	end = omp_get_wtime();
	printf("Threads: %d Estimation for pi is: %lf in %lf seconds with an error of %lf.\n"
               ,threads,pi,end-start,fabs(pi - M_PI));
}
