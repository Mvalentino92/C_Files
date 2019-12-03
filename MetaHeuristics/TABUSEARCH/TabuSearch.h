#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define T 2
#define N 4

//A struct that will be sorted with qsort, by the value parameter (to be updated every iteration).
typedef struct 
{
	int a;
	int b;
	double value;
} Swap;

//For to tabu moves
typedef struct
{
	int toJ;
	int toVal;
	int life;
} TabuTo;

//For tabu neighbors
typedef struct
{
	int sNeighbor;
	int bNeighbor;
	int life;
} TabuNeighbors;

//A struct for the entire algorithm
typedef struct
{
	int* root;
	int* best;
	double rootVal;
	double bestVal;
	double** matrix;
	int n;
	int size;
	Swap* permutations;
	TabuTo* tabuToList;
	TabuNeighbors* tabuNeighborsList;
	int* I_Frequency;
	int MAX_ITER;
	int tExpire;
	int nExpire;
} TB_Structure;

//A one time generation of the possible swaps for each iteration.
Swap* generatePermutations(int s, int t)
{
	int i, j, k, n, size;
	Swap *ps;

	n = t - s + 1;
	size = (n*n - n)/2;
	ps = (Swap *)malloc(sizeof(Swap)*size);
	k = 0;

	for(i = s; i < t; i++)
	{
		for(j = i + 1; j <= t; j++)
		{
			ps[k].a = i;
			ps[k++].b = j;
		}
	}
	return ps;
}

void TB_Init(TB_Structure* theModel, int* root, double rootVal, double** matrix,
	     int MAX_ITER, int tExpire, int nExpire, int s, int t, int n)
{
	int i,j,k;

	//Manually deepclone the root solution
	theModel->best = (int *)malloc(sizeof(int)*n);
	for(i = 0; i < n; i++) theModel->best[i] = root[i];

	//Given information from the function
	theModel->root = root;
	theModel->rootVal = rootVal;
	theModel->bestVal = rootVal;
	theModel->matrix = matrix;
	theModel->MAX_ITER = MAX_ITER;
	theModel->tExpire = tExpire;
	theModel->nExpire = nExpire;
	theModel->n = n;

	//Calculate size based on the bounds given
	theModel->size = ((t - s + 1)*(t - s + 1) - t + s - 1)/2;

	//Generate the permutations
	theModel->permutations = generatePermutations(s,t);	

	//Initialize the TabuLists with all Infinities and incremented life
	theModel->tabuToList = (TabuTo *)malloc(sizeof(TabuTo)*tExpire*T);
	k = 1;
	for(i = 0; i < tExpire*T; i += T) 
	{
		theModel->tabuToList[i] = (TabuTo){INT_MAX,INT_MAX,k};
		theModel->tabuToList[i+1] = (TabuTo){INT_MAX,INT_MAX,k++};
	}
	theModel->tabuNeighborsList = (TabuNeighbors *)malloc(sizeof(TabuNeighbors)*nExpire*N);
	k = 1;
	for(i = 0; i < nExpire*N; i += N) 
	{
		theModel->tabuNeighborsList[i] = (TabuNeighbors){INT_MAX,INT_MAX,k};
		theModel->tabuNeighborsList[i+1] = (TabuNeighbors){INT_MAX,INT_MAX,k};
		theModel->tabuNeighborsList[i+2] = (TabuNeighbors){INT_MAX,INT_MAX,k};
		theModel->tabuNeighborsList[i+3] = (TabuNeighbors){INT_MAX,INT_MAX,k++};
	}

	//Initialize and 0 out the matrix for the Index Value Frequency
	theModel->I_Frequency = (int *)malloc(sizeof(int)*n);
	for(i = 0; i < n; i++) 
	{
		theModel->I_Frequency[i] = 0;
	}
}

//Compares the fitness values in a Swap struct (passed to qsort)
int cmpFitness(const void *a, const void *b)
{
	Swap *sa = (Swap *)a;
	Swap *sb = (Swap *)b;
	return sa->value - sb->value <  0 ? -1 : 1;
}

//Compares the the equality of a TabTo struct (index and value match)
int cmpTabuTo(const void * a, const void * b)
{
	TabuTo * pa = (TabuTo *)a;
	TabuTo * pb = (TabuTo *)b;
	if(pa->toJ == pb->toJ) return pa->toVal - pb->toVal;
	return pa->toJ - pb->toJ;
}

//Compares the equality of a TabuNeighbor struct (smaller neighbor to larger neighbor)
int cmpTabuNeighbor(const void * a, const void * b)
{
	TabuNeighbors * pa = (TabuNeighbors *)a;
	TabuNeighbors * pb = (TabuNeighbors *)b;
	if(pa->sNeighbor == pb->sNeighbor) return pa->bNeighbor - pb->bNeighbor;
	return pa->sNeighbor - pb->sNeighbor;
}

//The function that seeks to find a best neighbor in the neighborhood
int foundBestNeighbor(TB_Structure* theModel,int iter)
{
	int i,j,k,l,a,b,x0,x1,y0,y1,aval,bval,temp,recency,frequency,tFound,nFound,s,bg,ftol,ktol;
	TabuTo ab, ba,*bToResult, tempTo;
	TabuNeighbors x0b,x1b,y0a,y1a,*bNeighborResult, tempNeighbor;
	TabuTo winnerTo[T];
	TabuNeighbors winnerNeighbor[N];
	ftol = theModel->n/2;
	ktol = ftol - 1;

	//For every swap in the permutations
	for(i = 0; i < theModel->size; i++)
	{
		//Save spots for easy access
		a = theModel->permutations[i].a;
		b = theModel->permutations[i].b;

		//Save the value of this index for easy access
		aval = theModel->root[a];
		bval = theModel->root[b];

		/*Try to grab the neighbors currently next to a before the swap
		 * If the neighbor doesn't exist (at endpoints), mark with a -1.*/
		x0 = a-1 > -1 ? theModel->root[a-1] : -1;
		x1 = a+1 < theModel->n ? theModel->root[a+1] : theModel->n;

		y0 = b-1 > -1 ? theModel->root[b-1] : -1;
		y1 = b+1 < theModel->n ? theModel->root[b+1] : theModel->n;

		//Create the structs for TabuTo (Index and value, after swap)
		ab = (TabuTo){a,bval,0};
		ba = (TabuTo){b,aval,0};

		/*Create the TabuNeighbor structs. For efficient equality checks and sorting
		 * Have the smaller neighbor always stored first*/
		if(x0 <= bval) x0b = (TabuNeighbors){x0,bval,theModel->nExpire};
		else x0b = (TabuNeighbors){bval,x0,theModel->nExpire};

		if(x1 <= bval) x1b = (TabuNeighbors){x1,bval,theModel->nExpire};
		else x1b = (TabuNeighbors){bval,x1,theModel->nExpire};

		if(y0 <= aval) y0a = (TabuNeighbors){y0,aval,theModel->nExpire};
		else y0a = (TabuNeighbors){aval,y0,theModel->nExpire};

		if(y1 <= aval) y1a = (TabuNeighbors){y1,aval,theModel->nExpire};
		else y1a = (TabuNeighbors){aval,y1,theModel->nExpire};

		//Init recency as 0, use binary search to find if they are in the list.
		recency = 0;
		bToResult = (TabuTo *)bsearch(&ab,theModel->tabuToList,
			     theModel->tExpire*T,sizeof(TabuTo),cmpTabuTo);
		if(bToResult != NULL) recency++;
		bToResult = (TabuTo *)bsearch(&ba,theModel->tabuToList,
			     theModel->tExpire*T,sizeof(TabuTo),cmpTabuTo);
		if(bToResult != NULL) recency++;

		//NeighborList
		bNeighborResult = (TabuNeighbors *)bsearch(&x0b,theModel->tabuNeighborsList,
				          theModel->nExpire*N,sizeof(TabuNeighbors),cmpTabuNeighbor);
		if(bNeighborResult != NULL) recency++;
		bNeighborResult = (TabuNeighbors *)bsearch(&x1b,theModel->tabuNeighborsList,
				          theModel->nExpire*N,sizeof(TabuNeighbors),cmpTabuNeighbor);
		if(bNeighborResult != NULL) recency++;
		bNeighborResult = (TabuNeighbors *)bsearch(&y0a,theModel->tabuNeighborsList,
				          theModel->nExpire*N,sizeof(TabuNeighbors),cmpTabuNeighbor);
		if(bNeighborResult != NULL) recency++;
		bNeighborResult = (TabuNeighbors *)bsearch(&y1a,theModel->tabuNeighborsList,
				          theModel->nExpire*N,sizeof(TabuNeighbors),cmpTabuNeighbor);
		if(bNeighborResult != NULL) recency++;

		//****************DOUBLE CHECK THE FREQUENCY ONE****/
		frequency = 0;
		for(j = 0; j < theModel->n; j++)
		{
			frequency += theModel->I_Frequency[j] >= ftol ? 1 : 0;
		}
		if(theModel->I_Frequency[a] >= ftol) frequency -= 1;
		if(theModel->I_Frequency[b] >= ftol) frequency -= 1;
		
		//Did it! (Update everything, add stuff to lists etc)
		if(1 || (recency <= 2 && frequency < ktol))
		{
			//Minus 1 from everyones value
			for(j = 0; j < theModel->tExpire*T; j++) theModel->tabuToList[j].life--;
			for(j = 0; j < theModel->nExpire*N; j++) theModel->tabuNeighborsList[j].life--;

			//Create the values to be added to the tabuToList, add them to list and sort
			winnerTo[0] = (TabuTo){a,aval,theModel->tExpire};
			winnerTo[1] = (TabuTo){b,bval,theModel->tExpire};
			
			//Add values to winner for neighbors and sort
			winnerNeighbor[0] = x0b;
			winnerNeighbor[1] = x1b;
			winnerNeighbor[2] = y0a;
			winnerNeighbor[3] = y1a;

			//Find the indices, add the new value, and bubblesort in the correct direction.
			j = 0;
			tFound = 0;
			while(tFound < T)
			{
				if(theModel->tabuToList[j].life == 0)
				{
					theModel->tabuToList[j] = winnerTo[tFound];

					//Bubblesort in whatever direction
					l = j;
					k = j - 1;
					while(k > -1 && cmpTabuTo(&(theModel->tabuToList[l]),&(theModel->tabuToList[k])) < 0)
					{
						//Swap them
						tempTo = theModel->tabuToList[l];
						theModel->tabuToList[l] = theModel->tabuToList[k];
						theModel->tabuToList[k] = tempTo;
						l--;
						k--;
					}

					l = j;
					k = j + 1;
					while(k < theModel->tExpire*T && cmpTabuTo(&(theModel->tabuToList[l]),&(theModel->tabuToList[k])) > 0)
					{
						//Swap them
						tempTo = theModel->tabuToList[l];
						theModel->tabuToList[l] = theModel->tabuToList[k];
						theModel->tabuToList[k] = tempTo;
						l++;
						k++;
					}
					tFound++;
				}
				else j++;
			}

			//Find the indices, add the new value, and bubblesort in the correct direction.
			j = 0;
			nFound = 0;
			while(nFound < N)
			{
				if(theModel->tabuNeighborsList[j].life == 0)
				{
					theModel->tabuNeighborsList[j] = winnerNeighbor[nFound];

					//Bubblesort in whatever direction
					l = j;
					k = j - 1;
					while(k > -1 && cmpTabuNeighbor(&(theModel->tabuNeighborsList[l]),&(theModel->tabuNeighborsList[k])) < 0)
					{
						//Swap them
						tempNeighbor = theModel->tabuNeighborsList[l];
						theModel->tabuNeighborsList[l] = theModel->tabuNeighborsList[k];
						theModel->tabuNeighborsList[k] = tempNeighbor;
						l--;
						k--;
					}

					l = j;
					k = j + 1;
					while(k < theModel->nExpire*N && cmpTabuNeighbor(&(theModel->tabuNeighborsList[l]),&(theModel->tabuNeighborsList[k])) > 0)
					{
						//Swap them
						tempNeighbor = theModel->tabuNeighborsList[l];
						theModel->tabuNeighborsList[l] = theModel->tabuNeighborsList[k];
						theModel->tabuNeighborsList[k] = tempNeighbor;
						l++;
						k++;
					}
					nFound++;
				}
				else j++;
			}

			//Frequency update
			for(j = 0; j < theModel->n; j++) theModel->I_Frequency[j]++;
			theModel->I_Frequency[a] = 0;
			theModel->I_Frequency[b] = 0;

			//printf("Tabu Solutions: %d for iteration %d\n",i,iter);
			return i;
		}
	}
	return -1;
}

//Takes root solution, the fitness function (needs root solution, swap indices, and matrix of information)
//Max iterations to perform, and two expire values)
//And s and t, the bounds of the indices you want to swap
int * TB(int* root, double rootVal, double** matrix, double fitness(int*,double, int, int, int,double**)
	,int MAX_ITER, int tExpire, int nExpire, int s, int t, int n)
{
	//Init
	TB_Structure theModel;
	int i,j,k,swapDex,a,b,temp;
	
	//Construct theModel
	TB_Init(&theModel,root,rootVal,matrix,MAX_ITER,tExpire,nExpire,s,t,n);	

	//Iterations of main algorithm
	for(i = 0; i < theModel.MAX_ITER; i++)
	{
		//printf("ITERATION: %d\n",i);
		for(j = 0; j < theModel.size; j++)
		{
			theModel.permutations[j].value = fitness(theModel.root,
					            theModel.rootVal,
					            theModel.permutations[j].a,
						    theModel.permutations[j].b,
					            theModel.n,
					            theModel.matrix);
		}
		
		//Sort, based on value
		qsort(theModel.permutations,theModel.size,sizeof(Swap),cmpFitness);

		//Update everything, by finding first thing that isn't tabu
		//If true, keep going, if false return (no solution was found non-tabu)
		swapDex = foundBestNeighbor(&theModel,i+1);
		if(swapDex < 0) break;

		//Change the value of the permutation
		a = theModel.permutations[swapDex].a;
		b = theModel.permutations[swapDex].b;
		temp = theModel.root[a];
		theModel.root[a] = theModel.root[b];
		theModel.root[b] = temp;

		//Change the value of the cost
		theModel.rootVal = theModel.permutations[swapDex].value;

		//Update if a better value was found
		//printf("RootVal: %lf\tBestVal: %lf\n",theModel.rootVal,theModel.bestVal);
		if(theModel.rootVal < theModel.bestVal)
		{
			theModel.bestVal = theModel.rootVal;
			for(k = 0; k < n; k++) theModel.best[k] = theModel.root[k];
		}
		
	}
	if(i == MAX_ITER) printf("FINISHED");
	return theModel.best;
}
