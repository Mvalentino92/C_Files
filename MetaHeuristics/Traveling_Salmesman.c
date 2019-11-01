#include "TB_Helper.h"

//Function to permutate to evalualte the fitness
double fitness(int * root, double value, int a, int b, int n, double **matrix)
{
	//a and b are the indices to swap
	//Just do the naive swap
	int i,j,k,temp;
	double retval,xDiff,yDiff,minus,plus,checker;

	//Do minus
	minus = 0.0;
	xDiff = matrix[0][root[a-1]] - matrix[0][root[a]];
	yDiff = matrix[1][root[a-1]] - matrix[1][root[a]];
	minus += sqrt(xDiff*xDiff + yDiff*yDiff);
	xDiff = matrix[0][root[a+1]] - matrix[0][root[a]];
	yDiff = matrix[1][root[a+1]] - matrix[1][root[a]];
	minus += sqrt(xDiff*xDiff + yDiff*yDiff);

	xDiff = matrix[0][root[b-1]] - matrix[0][root[b]];
	yDiff = matrix[1][root[b-1]] - matrix[1][root[b]];
	minus += sqrt(xDiff*xDiff + yDiff*yDiff);
	xDiff = matrix[0][root[b+1]] - matrix[0][root[b]];
	yDiff = matrix[1][root[b+1]] - matrix[1][root[b]];
	minus += sqrt(xDiff*xDiff + yDiff*yDiff);

	//Swap
	temp = root[a];
	root[a] = root[b];
	root[b] = temp;
	
	//Do plus
	plus = 0.0;
	xDiff = matrix[0][root[a-1]] - matrix[0][root[a]];
	yDiff = matrix[1][root[a-1]] - matrix[1][root[a]];
	plus += sqrt(xDiff*xDiff + yDiff*yDiff);
	xDiff = matrix[0][root[a+1]] - matrix[0][root[a]];
	yDiff = matrix[1][root[a+1]] - matrix[1][root[a]];
	plus += sqrt(xDiff*xDiff + yDiff*yDiff);

	xDiff = matrix[0][root[b-1]] - matrix[0][root[b]];
	yDiff = matrix[1][root[b-1]] - matrix[1][root[b]];
	plus += sqrt(xDiff*xDiff + yDiff*yDiff);
	xDiff = matrix[0][root[b+1]] - matrix[0][root[b]];
	yDiff = matrix[1][root[b+1]] - matrix[1][root[b]];
	plus += sqrt(xDiff*xDiff + yDiff*yDiff);

	//Swap back
	temp = root[a];
	root[a] = root[b];
	root[b] = temp;

	return value - minus + plus;
}

int main(int argc, char ** argv)
{
	int i,j,k,tExpire,nExpire,s,t,MX_ITER,r,c,*root, temp,size;
	double sol, *rows, **matrix,xDiff,yDiff,finalSol;

	r = 2;
	c = 1001;
	matrix = (double **)malloc(sizeof(double *)*r);
	for(i = 0; i < r; i++) matrix[i] = (double *)malloc(sizeof(double)*c);

	s = 1;
	t = 999;
	size = pow((t - s + 1),2)/2;
	tExpire = size*0.05;
	nExpire = tExpire/2;
	MX_ITER = 2500;

	//Do all x, then all y
	k = 1;
	for(i = 0; i < r; i++)
	{
		for(j = 0; j < c; j++) 
		{
			matrix[i][j] = atof(argv[k++]);
		}
	}

	//Create root solution
	srand(time(NULL));
	root = (int *)malloc(sizeof(int)*c);
	for(i = 0; i < c; i++) root[i] = i;
	for(i = 0; i < c - 1; i++)
	{
		k = rand() % (c - i - 1) + i;
		temp = root[k];
		root[k] = root[i];
		root[i] = temp;
	}
	root[c-1] = root[0];
	sol = 0.0;
	for(k = 0; k < c - 1; k++)
	{
		xDiff = matrix[0][root[k+1]] - matrix[0][root[k]];
		yDiff = matrix[1][root[k+1]] - matrix[1][root[k]];
		sol += sqrt(xDiff*xDiff + yDiff*yDiff);
	}
        root = TB(root,sol,matrix, fitness,MX_ITER,tExpire,nExpire,s,t,c);
	finalSol = 0.0;
	for(k = 0; k < c - 1; k++)
	{
		xDiff = matrix[0][root[k+1]] - matrix[0][root[k]];
		yDiff = matrix[1][root[k+1]] - matrix[1][root[k]];
		finalSol += sqrt(xDiff*xDiff + yDiff*yDiff);
	}
	printf("\nInitial is: %f\tFinal is: %f\n",sol,finalSol);
}
