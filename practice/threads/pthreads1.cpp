// create 5 POSIX threads (p threads), and in their initial routine print "thread i is running"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// We will create 5 threads. So the total number of threads in the process will be 6 because the process already starts with one thread
#define NUM_THREADS 5    // no need for semicolon

//The start routine must have a void pointer parameter and return a void pointer
//So when passing in an argument in pthread_create, make sure to convert it to a void pointer using type casting
void * print_routine(void * tid) {
	printf("Thread %ld is running\n", (int64_t) tid);     // using 64 bit integer because or else it will cause error if we use (int)
	pthread_exit(0);   // status is normal, so it will be 0
}


int main() {

	// create an array of pthreads
	pthread_t thread[NUM_THREADS];

	int status;
	// use a for loop to get each thread to execute the start routine using pthread_create
	for (int64_t i = 0; i < NUM_THREADS; i++) {

		printf("Creating thread %ld\n", i);		

		// pthread_create takes in a pointer to a thread, thread attribute (usually NULL), the start routine (which has to have return type void* and
		// have a parameter of type void*), and any arg of that start routine
		status = pthread_create(&thread[i], NULL, print_routine, (void *) i);
		
		// if status > 0, then an error has occured and pthread_create did not work
		if (status > 0) {
			printf("Error code %d has occured, pthread_create for thread %ld did not work", status, i);
		}

	}

	// wait until all the threads have finished running, then end the program
	for (int i= 0; i < NUM_THREADS; i++) {
		pthread_join(thread[i], NULL);
	}

	exit(0);    // or just return 0;






	return 0;
}

















