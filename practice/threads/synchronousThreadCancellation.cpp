// Example of synchronous thread cancellation using a global flag


#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>


#define NUMBER_OF_THREADS 5

// Use a global variable for the cancellation flag
volatile int cancel_flag = 0;


void * thread_print(void * tid) {
	while(1) {
 		printf("thread %ld running\n",(long) tid);
 		sleep(1);

 		/* check if cancellation was requested */
		if (cancel_flag) {   // ie. if cancel_flag == 1
			return 0;			
		}

 	}
}


int main() {
 	pthread_t threads[NUMBER_OF_THREADS];
 	
	for (long i = 0; i < NUMBER_OF_THREADS; i++) {
 		if( 0 != pthread_create(& threads[i], NULL, thread_print, (void *) i)) {
 			printf("Oops, pthread_create failed.\n"); exit(-1);
 		}
 	}
 
	sleep(5); // pretend to do something
 	
	/* request cancellation of all the created flags by simply setting the cancel_flag to 1 */
	cancel_flag = 1;	

	// since we have cancelled all the created flags, this code is useless
 	for (long i = 0; i < NUMBER_OF_THREADS; i++) {
 		pthread_join(threads[i], NULL);
	}

	return 0;
}
