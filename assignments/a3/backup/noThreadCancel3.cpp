// ======================================================================
// You must modify this file and then submit it for grading to D2L.
// ======================================================================

#include "sumFactors.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>



int64_t sum;
int64_t nThreads;
int64_t num;
bool readSuc;
bool baseCase;            // need to reset baseCase at start of loop
int64_t smallestFactor;

pthread_mutex_t mutex1;
pthread_barrier_t barrier1;





void* startRoutine(void* arg) {

	int64_t tid = (int64_t) arg;
	int64_t res;

	int64_t start;
	int64_t end;
	int64_t max;


	// main loop, if thread exits this loop then it terminates
	while(1) {
		res = pthread_barrier_wait(&barrier1); 
		if (res == PTHREAD_BARRIER_SERIAL_THREAD) {
			
			sum += smallestFactor;                         // the smallestFactor of the previous iteration will be added to sum in this iteration
														   // In the first iteration, this will just add 0
			smallestFactor = 0;                            // Reset smallestFactor for every iteration after adding it to sum

			readSuc = (std::cin >> num) ? true : false;    // no need for mutex as all other threads are waiting
			//printf("%ld   %ld\n", num, sum);
		
			baseCase = false;        // reset baseCase flag before checking base cases
			if (readSuc) {
				if( num <= 3) {      // 2 and 3 are primes
					sum += 0;
					baseCase = true;
				}
		        else if( num % 2 == 0) { // handle multiples of 2
					sum += 2;
					baseCase = true;
				}
        		else if( num % 3 == 0) { // handle multiples of 3
					sum += 3;
					baseCase = true;
				}

			}

		}	// end of serial work

		pthread_barrier_wait(&barrier1);
		if (baseCase) continue;                  // If one of the base cases, then we already added to sum, so all threads				
                                                 // move to next iteration.

        if (!readSuc) break;                 	 // If read was unsuccessful, then we are done so all threads exit the loop


		// if the above 2 conditions do not hold, then we divide up the num and do parallel work
		max = sqrt(num);
		start = ((5 + (max-5)) * tid) / nThreads;
		while (!((start-5) % 6 == 0)) start++;             // make sure start is "aligned"

		end = ((5 + (max-5)) * (tid+1)) / nThreads;
		//printf("\n thread:%ld\n max:%ld [%ld, %ld]\n", tid, max, start, end);

		if (start > max) continue;                         // **important** makes sure 5 does not return 5 among other things


		// try to find the smallest factor
		for (int64_t i = start; i <= end; i += 6) {
			if (num % i == 0) {
				pthread_mutex_lock(&mutex1);
					if ((smallestFactor == 0) || (i < (smallestFactor))) smallestFactor = i;
				pthread_mutex_unlock(&mutex1);
				break;
			}

			if (num % (i+2) == 0) {
				pthread_mutex_lock(&mutex1);
					if ((smallestFactor == 0) || ((i+2) < (smallestFactor))) smallestFactor = i+2;
				pthread_mutex_unlock(&mutex1);
                break;

			}

		} 


	}// end of main loop

	pthread_exit(0);
}







int64_t sum_factors(int n_threads) {
	// initialize nThreads, barrier, mutex
	nThreads = n_threads;
    pthread_barrier_init(&barrier1, NULL, nThreads);
	pthread_mutex_init(&mutex1, NULL);

	// create threads, make threads run start routine with tid
	pthread_t thread[nThreads];
	for (int64_t i = 0; i < nThreads; i++) {
		pthread_create(&thread[i], NULL, startRoutine, (void*) i);
	}
	
	// wait for threads to finish executing
	for (int64_t i = 0; i < nThreads; i++) {
		pthread_join(thread[i], NULL);
	}

	// destroy mutex, barrier
	pthread_mutex_destroy(&mutex1);
	pthread_barrier_destroy(&barrier1);

	return sum;
}
