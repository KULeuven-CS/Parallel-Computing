void odd_even_par(float *array, int n){
	//array is a pointer to the array to sort.
	//n = length of the array and assumed a multiple of P(or p).
	bsp_begin(P);
	//Initialisation and data distribution superstep
	int p = bsp_nprocs(); 
	int s = bsp_pid();
	int localsize = sizeof(float)*n/p;
	int otherarray = malloc(localsize);
	int *localarray = malloc(localsize);
	for(int i=0; i<n/p; i++){
		localarray[i] = array[s+i*n/p];
	}
	bsp_push_reg(localarray, localsize);
	bsp_sync();

	//Initialisation superstep
	for(int i=1; i<=n; i++){
		if(!isEven(i)){ // i is odd
			if(!isEven(s)){ // pid is odd
				comp_exch_min(s+1, &localarray, &otherarray, s, p, localsize);
			} else { // pid is even
				comp_exch_max(s-1, &localarray, &otherarray, s, p, localsize);
			}
		} else { // i is even
			if(isEven(s)){ // pid is even
				comp_exch_min(s+1, &localarray, &otherarray, s, p, localsize);
			} else { // pid is odd 
				comp_exch_max(s-1, &localarray, &otherarray, s, p, localsize);
			}
		}

	}
}

int isEven(int i){
	return i % 2;	
}

//keep the only the minima in the local array
void comp_exch_min(int otherpid, float *myarray, float *otherarray, int s, int p){
	//Communication superstep: exchanging with neighboring processors.
	bsp_get(otherpid, localarray, 0, otherarray, localsize);
	bsp_sync();

	//Calcuclation superstep: comparing and updating the localarray.
	for(int i=0; i<n/p; i++){
		if(localarray[i] > otherarray[i]){
			localarray[i] = otherarray[i];
		}
	}
	bsp_sync();
}

//keep the only the maxima in the local array
void comp_exch_max(int otherpid){
	//Communication superstep: exchanging with neighboring processors.
	bsp_get(otherpid, localarray, 0, otherarray, localsize);
	bsp_sync();

	//Calcuclation superstep: comparing and updating the localarray.
	for(int i=0; i<n/p; i++){
		if(localarray[i] < otherarray[i]){
			localarray[i] = otherarray[i];
		}
	}
	bsp_sync();
}
