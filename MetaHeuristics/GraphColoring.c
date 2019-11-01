#include "TBP_Helper.h"

void swapLabel(double **G, int V, int i, int j)
{
	int k;
	double temp;
	for(k = 0; k < V; k++)
	{
		if(k == i || k == j) continue;
		else
		{
			temp = G[k][i];
			G[k][i] = G[k][j];
			G[k][j] = temp;

			temp = G[i][k];
			G[i][k] = G[j][k];
			G[j][k] = temp;
		}
	}
}
	
double fitness(int * root, double value, int a, int b, int n, double **matrix)
{
	int i,j,k,l,temp,have,want,*original,*originalBack;
	double retval;
	//The matrix is by the default the original ordering representation of 0,1,2,3..n
	//So do all the swaps neccessary, and then undo them. 
	
	//Declare stuff
	retval = 0.0;
	original = (int *)malloc(sizeof(int)*n);
	originalBack = (int *)malloc(sizeof(int)*n);

	//Swap the things in root.
	temp = root[a];
	root[a] = root[b];
	root[b] = temp;

	//Begin to change from the default ordering of 0,1,2,3,n, to the new permutation which is root.
	for(i = 0; i < n; i++) 
	{
		original[i] = i;
		originalBack[i] = i;
	}

	for(i = 0; i < n; i++)
	{
		//Look at what we want
		want = root[i];

		//See what we have
		have = original[i];

		//If we are in the right spot, dont do anything.
		//If we aren't, then iterate forward until we find what we want and swap
		if(want != have)
		{
			for(j = i + 1; j < n; j++)
			{
				if(want == original[j]) 
				{
					swapLabel(matrix,n,i,j);
					temp = original[i];
					original[i] = original[j];
					original[j] = temp;
				}
			}
		}
	}

	//Evaulate this solution
	i = 0; 
	j = 0;

	while(i < n)
	{
		for(k = 0; k < n - i; k++)
		{
			//Check if we have 0's, if we dont, stop
			if(matrix[i][j+k] != 0 || matrix[i+k][j] != 0) break;
			else
			{
				for(l = 1; l <= k; l++)
				{
					if(matrix[i+l][j+k] != 0 || matrix[i+k][j+l] != 0) break;
				}
				if(l <= k) break;
			}
		}
		retval += 1.0;
		i += k;
		j += k;
	}

	//Swap back
	for(i = 0; i < n; i++)
	{
		//Look at what we want
		want = originalBack[i];

		//See what we have //This is now root
		have = original[i];

		//If we are in the right spot, dont do anything.
		//If we aren't, then iterate forward until we find what we want and swap
		if(want != have)
		{
			for(j = i + 1; j < n; j++)
			{
				if(want == original[j]) 
				{
					swapLabel(matrix,n,i,j);
					temp = original[i];
					original[i] = original[j];
					original[j] = temp;
				}
			}
		}
	}

	//Swap root back
	temp = root[a];
	root[a] = root[b];
	root[b] = temp;

	free(original);
	free(originalBack);

	return retval;
}

int main(int argc, char ** argv)
{
	int i,j,k,tExpire,nExpire,MX_ITER,s,t,sol,finalSol,V,*root,size;
	double **matrix;
	FILE *fptr; 

	//Get number of vertices and read in file
	V = atoi(argv[1]);
	fptr = fopen(argv[2],"r");	

	//Begin to fill the matrix, (the adjacency matrix)
	matrix = (double **)malloc(sizeof(double *)*V);
	for(i = 0; i < V; i++)
	{
		matrix[i] = (double *)malloc(sizeof(double)*V);
		for(j = 0; j < V; j++) fscanf(fptr,"%f",&(matrix[i][j]));
	}

	//Close file
	fclose(fptr);

	//Create the root solution (by default just 0 1 2 3 4 ... V) 
	root = (int *)malloc(sizeof(int)*V);
	for(i = 0; i < V; i++) root[i] = i;

	//The default solution value is just going to be V (not true of the matrix, but its the upper bound)
	sol = V;

	//Declare other essentials
	s = 0;
	t = V-1;
	size = pow((t - s + 1),2)/2;
	tExpire = size*0.05;
	nExpire = tExpire/2;
	MX_ITER = 100;

	//TEST
	//printf("%d\n",(int)fitness(root,0,0,0,V,matrix));

	//Pass everything to the algorithm
	root = TB(root,sol,matrix,fitness,MX_ITER,tExpire,nExpire,s,t,V);
	printf("%d\n",(int)fitness(root,0,0,0,V,matrix));
}
