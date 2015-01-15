#include <stdlib.h>

void odd_even_par(float *array, int n){
	//array is a pointer to the array to sort.
	//n = length of the array and amount of used processors. 
	//n is assumed to be a multiple of p.
	bsp_begin(P);
	//Initialisation and data distribution superstep 0
	int p = P; 
	int s = bsp_pid();
	int chunklength = n/p; 
	int chunksize = sizeof(float)*chunklength;
	//Allocate 2 times the chunk size for the local array to avoid copying memory.
	int *localarray = malloc(chunksize);
	int *otherarray = malloc(chunksize);
	for(int i=0; i<n/p; i++){
		localarray[i] = array[s+i*n/p];
	}
	bsp_push_reg(localarray, chunksize);
	bsp_sync();

	//Calculation superstep for first local sorting
	qsort(localarray, chunksize, sizeof(float), compare_floats_asc);

	for(int i=1; i<=n; i++){ // n times the superstep 3 and 4
		if(!isEven(i)){ // i is odd
			if(!isEven(s)){ // pid is odd
				comp_split_min(s+1, localarray, otherarray, chunksize, chunklength);
			} else { // pid is even
				comp_split_max(s-1, localarray, otherarray, chunksize, chunklength);
			}
		} else { // i is even
			if(isEven(s)){ // pid is even
				comp_split_min(s+1, localarray, otherarray, chunksize, chunklength);
			} else { // pid is odd 
				comp_split_max(s-1, localarray, otherarray, chunksize, chunklength);
			}
		}

	}
}

int isEven(int i){
	return i % 2;	
}

//keep the only the minima in the local array
void comp_split_min(int otherpid, float *myarray, float *otherarray, int chunksize, int chunklength){
	//Communication superstep 3: get other chuck and store it in the second part of the localarray 
	bsp_get(otherpid, localarray, 0, otherarray, chunksize);
	bsp_sync();

	//Calcuclation superstep 4: local sort and split with neighboring processors.
	//now the lower half of localarray has the correct (lowest) values.
	int t=0;
	float localarray=myarray, tmp=malloc(chunksize);
	while(t<chunklength){
		if(*localarray <= *otherarray){
			tmp[t] = *localarray;
			localarray++;
			t++;
		} else {
			tmp[t] = *otherarray;
			otherarray++;
			t++;
		}
	}
	myarray = tmp;
	bsp_sync();
}

//keep the only the maxima in the local array
void comp_split_max(int otherpid, float *myarray, float *otherarray, int chunksize, int chunklength){
	//Communication superstep 3: 
	bsp_get(otherpid, myarray, 0, myarray+chunklength, chunksize);
	bsp_sync();

	//Calcuclation superstep 4: local sort and split with neighboring processors.
	//now the lower half of localarray has the correct (highest) values.
	int t=0;
	float localarray=myarray+chunklength, oarray=otherarray+chunklength, tmp=malloc(chunksize);
	while(t<chunklength){
		if(*localarray >= *oarray){
			tmp[chunklength-t] = *localarray;
			localarray--;
			t++;
		} else {
			tmp[chunklength-t] = *otherarray;
			otherarray--;
			t++;
		}
	}
	myarray = tmp;
	bsp_sync();
}

//Support function for making a ascending comparison.
int compare_floats_asc (const float *a, const float *b) {
    if (*a > *b)
       return 1;
    else if (*a < *b)
       return -1;
    else
       return 0;
}
