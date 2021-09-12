//
//	Assignment 4
//	Name: Ayman Shahriar
//	Tutorial: T03      UCID: 10180260
//
//


#include "scheduler.h"
#include "common.h"
#include <queue>
#include <unistd.h>
#include <climits>

/*
struct Process {
 	int id;
 	int64_t arrival_time; 
 	int64_t burst; 
 	int64_t start_time; 
	int64_t finish_time;
};
*/

void simulate_rr( int64_t quantum, int64_t max_seq_len, std::vector<Process> & processes, std::vector<int> & seq) {
   
	// clear sequence, make copy of bursts, create ready queue (will store index, not the id)
    seq.clear();
	int64_t bursts[processes.size()];
	for (int i = 0; i < (int)processes.size(); i++) bursts[i] = processes[i].burst; 
	std::queue<int> readyQueue = std::queue<int>();
	

	// cpu will store index of running process (-1 if free), index will store index of incoming process, processesLeft stores # unfinished processes
	// timeSlice will store remaining time slice
	int cpu = -1;
	int index = 0;
	int64_t timeSlice = quantum;
	int processesLeft = processes.size();
	int64_t currentTime = 0;

	// used to implement adding -1 to seq
	bool wasIdle = false;

	// used for calculating next important event
    int64_t nextArrival;
    int64_t nextDoneorExceeded;
	int64_t earliestEvent;


	while (processesLeft > 0) {
		// if process in cpu and finished it's bursts, add id to seq, add finish time, set cpu=-1, decrement processesLeft, reset timeSlice
		if ((cpu > -1) && (bursts[cpu] == 0)) {
			if (currentTime == 31) printf("first\n");
			if ((int64_t)seq.size() < max_seq_len) seq.push_back(processes[cpu].id);
			processes[cpu].finish_time = currentTime;
			cpu = -1;
			processesLeft--;
			timeSlice = quantum;
			continue;
		}

		// if process in cpu and has exceeded time slice, add id to seq, add index to ready queue, set cpu = -1, reset timeSlice
		if ((cpu > -1) && (timeSlice == 0)) {
			if (currentTime == 31) printf("second      %ld\n", readyQueue.size());
			//if (!readyQueue.empty()) {
				if (currentTime == 31) printf("\nreached here\n");
				if ((int64_t)seq.size() < max_seq_len) seq.push_back(processes[cpu].id);
				readyQueue.push(cpu);
				cpu = -1;
				timeSlice = quantum;
			//}	
			 //However if there is nothing in ready queue, let the process continue to run by resetting time slice
			

			continue;
		}

		// If new process will arrive now, add new process to readyQueue, increment index
		if ((index < (int)processes.size()) && (processes[index].arrival_time == currentTime)) {
			if (currentTime == 31) printf("third\n");
			readyQueue.push(index);
			index++;
			continue;
		}

		// If cpu is idle and ready queue is not empty, pop a process index from ready queue into cpu
		if ((cpu == -1) && (!readyQueue.empty())) {
			if (currentTime == 31) printf("fourth\n");
			cpu = readyQueue.front();
			readyQueue.pop();
			timeSlice = quantum;
			// if this is the first time process is running, update start time
			if (processes[cpu].start_time == -1) {
				processes[cpu].start_time = currentTime;
			}

			// if cpu was idle before, then add -1 to seq, set idle to false
			if (wasIdle) {
				if ((int64_t)seq.size() < max_seq_len) seq.push_back(-1);
				wasIdle = false;
			}			

			continue;
		}




		// update time to closest of three things: new process is arriving, process on cpu is done, process has exceeded time slice
		if (cpu > -1) {
			if (currentTime == 31) printf("fifth\n");
			// if ready queue is empty and no more processes arriving
            if ((index >= (int) processes.size()) && (readyQueue.empty())) {
                currentTime += bursts[cpu];
                bursts[cpu] = 0;
                continue;
            }

            // time of new process arriving
            if (index < (int)processes.size()) nextArrival = processes[index].arrival_time;
			else nextArrival = LLONG_MAX;

			// time of process finishing/exceeding time slice
			
			if (readyQueue.empty()) {      // it has to be that new process is arriving
				earliestEvent = std::min(nextArrival + (quantum-((nextArrival-currentTime)%quantum)), bursts[cpu]);
				if (earliestEvent == 31) {
					printf("its 31 \n");
				}

				for (int i = index; i < (int) processes.size(); i++) {
                	if (processes[i].arrival_time < earliestEvent) {
                    	readyQueue.push(i);
                    	index++;
                }
            }

			}			
			else {
				nextDoneorExceeded = std::min(bursts[cpu], timeSlice) + currentTime;
				earliestEvent = std::min(nextArrival, nextDoneorExceeded);
			}

			// determine earliest important event
			//earliestEvent = std::min(nextArrival, nextDoneorExceeded);			

			// update bursts, timeSlice, current time
			bursts[cpu] -= earliestEvent-currentTime;
			timeSlice -= earliestEvent-currentTime;
			if (timeSlice < 0) timeSlice = 0;          
			currentTime = earliestEvent;
        	
		}
        
		else {
			wasIdle = true; //(used for implementing -1 in seq)
			// it has to be the case that there is a new process arriving, and we don't have to calculate nextDoneorExceeded since cpu is idle
			currentTime = processes[index].arrival_time;
		}


	} 

}



//(make sure to add timeSlice-- and bursts[cpu]-- at the end)

// which is more efficient, checking each time if seq.size() <= max_seq_len , or adding and truncating so that seq.size() < max_seq_len


        //sleep(1);
        //printf("\nnextArrival: %ld     nextDoneorExceeded: %ld\n, current time: %ld", nextArrival, nextDoneorExceeded, currentTime);


