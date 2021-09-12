// ======================================================================
// Assignment 5
// Ayman Shahriar    UCID: 10180260       TUTORIAL: TO3
//
// This program implements both thread reuse and thread cancellation
//
// ======================================================================

#include "sumFactors.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
//#include <climits>
#include <limits>
#include <atomic>


int64_t sum;
int64_t nThreads;
int64_t num;
bool readSuc;
bool baseCase;            // need to reset baseCase at start of loop
int64_t max;
std::atomic<int64_t> smallestFactor(std::numeric_limits<int64_t>::max());


//pthread_mutex_t mutex1;
pthread_barrier_t barrier1;


template<typename T>
void update_min(std::atomic<T>& av, T const& v) noexcept {
  T prev = av;
  while(prev > v && ! av.compare_exchange_weak(prev, v)) ;
}


void* startRoutine(void* arg) {

	int64_t tid = (int64_t) arg;
	int64_t res;

	int64_t start;
	int64_t end;


	// main loop, if thread exits this loop then it terminates
	while(1) {
		res = pthread_barrier_wait(&barrier1); 
		if (res == PTHREAD_BARRIER_SERIAL_THREAD) {
			
			if (smallestFactor != std::numeric_limits<int64_t>::max()) {   	// the smallestFactor of the previous iteration wil be added to sum in this iteration
				sum += smallestFactor;                                                                  										
				smallestFactor = std::numeric_limits<int64_t>::max();        	// Reset smallestFactor for every iteration after adding it to sum
			}

			readSuc = (std::cin >> num) ? true : false;    // no need for mutex as all other threads are waiting
		
			baseCase = false;        // reset baseCase flag before checking base cases
			if (readSuc) {
				//printf("num: %ld\n", num);
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
				else {
					max = sqrt(num);
				}

			}

		}	// end of serial work

		pthread_barrier_wait(&barrier1);
		if (baseCase) continue;                  // If one of the base cases, then we already added to sum, so all threads				
                                                 // move to next iteration.

        if (!readSuc) break;                 	 // If read was unsuccessful, then we are done so all threads exit the loop


		// if the above 2 conditions do not hold, then we divide up the num and do parallel work
		start = ((5 + (max-5)) * tid) / nThreads;
		while (!((start-5) % 6 == 0)) start++;             // make sure start is "aligned"

		end = ((5 + (max-5)) * (tid+1)) / nThreads;
		//printf("Thread:%ld  [%ld,%ld]\n", tid, start, end);		
		if (start > max) continue;                         // **important** makes sure 5 does not return 5 among other things


		// try to find the smallest factor
		for (int64_t i = start; ((i <= end) && (i < smallestFactor)); i += 6) {
			if (num % i == 0) {
				update_min(smallestFactor, i);
				break;
			}

			if (num % (i+2) == 0) {
				update_min(smallestFactor, i+2);
                break;

			}

		} 


	}// end of main loop

	pthread_exit(0);
}







int64_t sum_factors(int n_threads) {
	// initialize nThreads, mutex
	nThreads = n_threads;
    pthread_barrier_init(&barrier1, NULL, nThreads);

	// create threads, make threads run start routine with tid
	pthread_t thread[nThreads];
	for (int64_t i = 0; i < nThreads; i++) {
		pthread_create(&thread[i], NULL, startRoutine, (void*) i);
	}
	
	// wait for threads to finish executing
	for (int64_t i = 0; i < nThreads; i++) {
		pthread_join(thread[i], NULL);
	}

	// destroy barrier
	pthread_barrier_destroy(&barrier1);

	return sum;
}
