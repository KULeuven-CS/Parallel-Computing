void odd_even_par(float *array, int n){
	//array is a pointer to the array to sort.
	//n = length of the array and amount of used processors. 
	bsp_begin(n);
	//Initialisation and data distribution superstep 0
	int p = n; 
	int s = bsp_pid();
	float *localelem, *otherelem;
	for(int i=0; i<n; i++){
		localelem = array[i];
	}
	bsp_push_reg(localelem, sizeof(float));
	bsp_sync();

	for(int i=1; i<=n; i++){ // n times the superstep 1 and 2
		if(!isEven(i)){ // i is odd
			if(!isEven(s)){ // pid is odd
				comp_exch_min(s+1, localelem, otherelem);
			} else { // pid is even
				comp_exch_max(s-1, localelem, otherelem);
			}
		} else { // i is even
			if(isEven(s)){ // pid is even
				comp_exch_min(s+1, localelem, otherelem);
			} else { // pid is odd 
				comp_exch_max(s-1, localelem, otherelem);
			}
		}

	}
}

int isEven(int i){
	return i % 2;	
}

//keep the only the minima in the local array
void comp_exch_min(int otherpid, float *myelem,  float *otherelem){
	//Communication superstep 1: exchanging with neighboring processors.
	bsp_get(otherpid, localelem, 0, otherelem, sizeof(float));
	bsp_sync();

	//Calcuclation superstep 2: comparing and updating the localarray.
	if(*myelem > *otherelem ){
		*myelem = *otherelem;
	}
	bsp_sync();
}

//keep the only the maxima in the local array
void comp_exch_max(int otherpid){
	//Communication superstep 1: exchanging with neighboring processors.
	bsp_get(otherpid, localelem, 0, otherelem, sizeof(float));
	bsp_sync();

	//Calcuclation superstep 2: comparing and updating the localarray.
	if(*myelem < *otherelem ){
		*myelem = *otherelem;
	}
	bsp_sync();
}
