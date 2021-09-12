//
//	Assignment 4
//
//	Author: Ayman Shahriar    ID: 10180260
//	Tutorial: T03      
//
//	Citation (APA format for lecture notes):
//	Federl, P. (2020). C++ implementation of the pseudocode considered in slide 14 of lecture slided titled "13b-scheduling" 
//      [Example program].  Retrieved from https://docs.google.com/presentation/d/1BhzosB-Zbwa5QE-CJ8gkvJ432X8aoNtqGyCyLLBlGVg/
//                                         edit#slide=id.g5aa361e1ce_10_14
//
//


#include "scheduler.h"
#include "common.h"
#include <deque>
#include <unistd.h>
#include <climits>


// This struct is kept here as reference, it is already defined in common.cpp
/*
struct Process {
 	int id;
 	int64_t arrival_time; 
 	int64_t burst; 
 	int64_t start_time; 
	int64_t finish_time;
};
*/


//
// This function runs a Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrival_times, and bursts
// output:
//   seq[] - will contain the execution sequence but trimmed to max_seq_len size
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//

void simulate_rr( int64_t quantum, int64_t max_seq_len, std::vector<Process> & processes, std::vector<int> & seq) {
   
	// clear sequence, make copy of bursts, create ready queue (will store index, not the id)
    seq.clear();
	int64_t bursts[processes.size()];
	for (int i = 0; i < (int)processes.size(); i++) bursts[i] = processes[i].burst; 
	std::deque<int> readyQueue = std::deque<int>();
	

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


	// used for calculating final optimization (also using earliestEvent)
	int64_t numStartTime = 0;
	int64_t earliestEndTime;
	int64_t arrivalTime;
	int64_t N;
	int64_t Increment;
	int64_t minBurst;



	while (processesLeft > 0) {
		// if process in cpu and finished it's bursts, add id to seq, add finish time, set cpu=-1, decrement processesLeft, reset timeSlice
		if ((cpu > -1) && (bursts[cpu] == 0)) {
			if ((int64_t)seq.size() < max_seq_len) seq.push_back(processes[cpu].id);
			processes[cpu].finish_time = currentTime;
			cpu = -1;
			processesLeft--;
			timeSlice = quantum;
			numStartTime--;    
			continue;
		}

		// if process in cpu and has exceeded time slice, add id to seq, add index to ready queue, set cpu = -1, reset timeSlice
		if ((cpu > -1) && (timeSlice == 0)) {
			if (!readyQueue.empty()) {
				if ((int64_t)seq.size() < max_seq_len) seq.push_back(processes[cpu].id);
				readyQueue.push_back(cpu);
				cpu = -1;
				timeSlice = quantum;
			}	

			// However if there is nothing in ready queue, let the process continue to run by resetting time slice
			else {
				timeSlice = quantum;
			}			

			continue;
		}

		// If new process will arrive now, add new process to readyQueue, increment index
		if ((index < (int)processes.size()) && (processes[index].arrival_time == currentTime)) {
			readyQueue.push_back(index);
			index++;
			continue;
		}


		// This is the "difficult" optimization that runs several processes at the same time
		// last conditional is to make sure we don't have any repeating last entries in seq	
		if (((cpu == -1) && (!readyQueue.empty())) && (numStartTime == (int)readyQueue.size()) && (readyQueue.size() > 1)) {
			// find earliest end time
			earliestEndTime = -1;
			minBurst = LLONG_MAX;
			for (auto p : readyQueue) {
				if ((bursts[p]) < minBurst) {
					minBurst = bursts[p];	
				}
			}
			
			earliestEndTime = (minBurst*readyQueue.size())-readyQueue.size();
			earliestEndTime += currentTime;
			arrivalTime = (index < (int)processes.size()) ? processes[index].arrival_time : LLONG_MAX;

			if (((int)readyQueue.size()*quantum < earliestEndTime-currentTime) && ((int)readyQueue.size()*quantum < arrivalTime-currentTime)) {

				earliestEvent = std::min(earliestEndTime, arrivalTime);
				N = (earliestEvent-currentTime)/(readyQueue.size()*quantum);         // -1 is to make sure N != earliestEvent				
				Increment = N*readyQueue.size()*quantum;
				currentTime += Increment;

				for (int p : readyQueue) {
					bursts[p] -= N*quantum;
					timeSlice = 0;         // this is redundant, but "just in case"					
				} 

				for (int i = 0; (i < N)&&((int)seq.size() < max_seq_len); i++) {
					for (int j = 0; (j < (int)readyQueue.size())&&((int)seq.size() < max_seq_len); j++) {
						seq.push_back(processes[readyQueue[j]].id);
	                }
				}
				continue;
			}
		}
			 
	

		// If cpu is idle and ready queue is not empty, pop a process index from ready queue into cpu
		if ((cpu == -1) && (!readyQueue.empty())) {
			cpu = readyQueue.front();
			readyQueue.pop_front();
			timeSlice = quantum;
			// if this is the first time process is running, update start time
			if (processes[cpu].start_time == -1) {
				processes[cpu].start_time = currentTime;
				numStartTime++;         //************
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
                earliestEvent = std::min(nextArrival + (quantum-((nextArrival-currentTime)%quantum)), bursts[cpu]+currentTime);

                for (int i = index; i < (int) processes.size(); i++) {
                    if (processes[i].arrival_time < earliestEvent) {
                        readyQueue.push_back(i);
                        index++;
                	}
            	}

            }
            else {
                nextDoneorExceeded = std::min(bursts[cpu], timeSlice) + currentTime;
                earliestEvent = std::min(nextArrival, nextDoneorExceeded);
            }

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



