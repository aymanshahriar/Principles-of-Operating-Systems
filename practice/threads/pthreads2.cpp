#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h>


#define NUM_THREADS 2

// global variables
int64_t N = 0;
int64_t oddTotal = 0;
int64_t evenTotal = 0;


// start routine for thread[0]. The function does not use any parameters
void* computeOdd(void* param) {
	for (int64_t i = 1; i <= N; i+=2) {
		oddTotal += i;
	}
	// we have computed the sum of odd numbers, so terminate this thread
	pthread_exit(0);
}


// start routine for thread[1]. The function does not use any parameters
void* computeEven(void* param) {
	for (int64_t i = 0; i <= N; i+=2) {
		evenTotal += i;
	}

	// we have computed the sum of even numbers, so terminate this thread
    pthread_exit(0);


}



int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("please enter number to compute as a command line argument\n");
		return 1;
	}
	
	// set global variable to number entered in command line
	N = atol(argv[1]);
	
	pthread_t thread[NUM_THREADS];

	int status1 = pthread_create(&thread[0], NULL, computeOdd, NULL);
	int status2 = pthread_create(&thread[1], NULL, computeEven, NULL);
	
	if ((status1 != 0) || (status2 != 0)) {
		printf("Something went wrong, one of the pthread_create did not work\n");
	}
	
	// wait for the two threads to finish executing;
	pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);

	int64_t total = oddTotal + evenTotal;
	printf("The sum of numbers 1..%ld is: %ld\n", N, total);


	return 0;
}






























