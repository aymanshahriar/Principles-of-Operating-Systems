//
// Name: Ayman Shahriar  UCID: 10180260  Tutorial:T03
//
//
// count_pi() calculates the number of pixels that fall into a circle
// using the algorithm explained here:
//
// https://en.wikipedia.org/wiki/Approximations_of_%CF%80
//
// count_pi() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create
//
// Currently the function ignores the n_threads parameter. Your job is to
// parallelize the function so that it uses n_threads threads to do
// the computation.

//#include "calcpi.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>



struct ParamStruct {
	int startRow;
	uint64_t skip;
	uint64_t partialCount;

	// should I make these global variables?
	int r;
	double rsq;
};


// Since the threads are not accessing any global variable, there is no race condition
void* startRoutine(void* arg) {
	// convert argument from void pointer to ParamStruct pointer
	ParamStruct* p = (struct ParamStruct*) arg;
	double skip = p->skip;
	int r = p->r;
	double rsq = p->rsq;

	uint64_t partialCount = 0;    // *** Instead of updating the partialCount in struct in every loop, it is much faster to
                                      // create a local variable and update that in every iteration of the loop, and after the loop you update the 
				      // partialCount struct
	//printf("%d\n", p->rsq);
	for( double x = p->startRow ; x <= r ; x += skip) {
        for( double y = -r ; y <= r ; y ++) 
            if( x*x + y*y <= rsq) partialCount ++;
	}

	p->partialCount = partialCount;
	
	pthread_exit(0);
}





uint64_t count_pi(int r, int n_threads) {   // remember that uint64_t = unsigned long
	double rsq = double(r) * r;
    uint64_t count = 0;

	// calculate number of rows
	int numRows = (2 * r) + 1;

	// if threads > number of rows, then we don't really need that many threads
	if (n_threads > numRows) {
		n_threads = numRows;
	}

	
	// Obtain the skip value, which is the number of threads
	// The skip value is used to divide the rows among the threads
	uint64_t skip = n_threads;
	
	// Initialize parameters to the start routine for each thread
	ParamStruct threadParam[n_threads];
	for (int i = 0; i < n_threads; i++) {
		threadParam[i].startRow = (i-r);
		threadParam[i].skip = skip;
		threadParam[i].r = r;
		threadParam[i].rsq = rsq;
	}
 	

	// Initialize the specified number of threads, make them run their start routines
	// If pthread_create fails, print error message
	pthread_t thread[n_threads];
	for (int i = 0; i < n_threads; i++) {
		if (pthread_create(&thread[i], NULL, startRoutine, &threadParam[i]) != 0) {
			printf("pthread_create() for %d did not work\n", i);
			return 0;
		}
	}



	// wait for threads to finish executing
	for (int i = 0; i < n_threads; i++) {
		pthread_join(thread[i], NULL);
	}

	// now add up all the partial count
	for (int i = 0; i < n_threads; i++) {
		count += threadParam[i].partialCount;
	}


  	return count;
}








