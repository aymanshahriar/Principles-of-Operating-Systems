// you need to modify this file

#include "scheduler.h"
#include "common.h"
#include <list>
#include <unistd.h> 

// this is the function you should edit
//
// runs Round-Robin scheduling simulator
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


/*
struct Process {
 int id;
 int64_t arrival_time, burst, start_time, finish_time;
};
*/


void simulate_rr(int64_t quantum, int64_t max_seq_len, std::vector<Process> & processes, std::vector<int> & seq) {
/*
    // Used to test how we are given each process
    for (Process &p : processes) {
        printf("\npid: %d  arrival: %ld    burst: %ld\n", p.id, p.arrival_time, p.burst);
        p.finish_time = p.arrival_time + p.burst;
        p.start_time = p.arrival_time;

        seq.push_back(p.id);
    }
*/


    // Clear the sequence
    seq.clear();

	// we will be modifying the burst of processes[], so create a copy of the burst
	std::vector<int64_t> Bursts = std::vector<int64_t>();
	for (Process p : processes) Bursts.push_back(p.burst);

	// Initialize a ready queue, which stores the index of processes that have "arrived" so far
	std::list<int> readyQueueIndexes = std::list<int>();
	for (int i = 0; i < (int)processes.size(); i++) readyQueueIndexes.push_back(i);
	
	int cpuIndex = -1;
    int64_t timeOnCPU;

	int jobsRemaining = (int)processes.size();
	int pointer = (int)processes.size();
	int64_t curr_time = 0;
	
	// Main loop that will loop until no more processes are left
	while(jobsRemaining > 0) {
		printf("\ncurrent process: %d", processes[cpuIndex].id);
		sleep(0.5);
		//if process in cpu is done, or has exceeded quantum
		if (cpuIndex != -1) {
			
			if (Bursts[cpuIndex] == 0) {
				// mark process done
				processes[cpuIndex].finish_time = curr_time;
				// set cpu idle
				cpuIndex = -1;
				// decrease jobsRemaining
				jobsRemaining--;
				// add process id to seq[]
				seq.push_back(processes[cpuIndex].id);
				timeOnCPU = 0;
				continue;
			}
		
			// if process has exceeded quantum
			if (timeOnCPU == quantum) {
				// add process id to seq[]
                seq.push_back(processes[cpuIndex].id);
				// put process index back to ready queue
				readyQueueIndexes.push_back(cpuIndex);
				// set cpu idle
				cpuIndex = -1;
				// reset time on cpu
				timeOnCPU = 0;
				continue;
			}
		}
	
		
		// if a new process arriving
		if (processes[pointer].arrival_time == curr_time ) {
			// add new process index to ready queue
			readyQueueIndexes.push_back(pointer);
			// update pointer to next index
			pointer--;
			continue;
		}
		
		//if cpu is idle 
		if (cpuIndex == -1) {
			// if ready queue is not empty
			if (!readyQueueIndexes.empty()) {
				//move process from RQ to CPU
				cpuIndex = readyQueueIndexes.front();
				readyQueueIndexes.pop_front();
				// set the start time
				processes[cpuIndex].start_time = curr_time;
				// reset time on cpu
				timeOnCPU = 0;
				continue;
			}
			
			// otherwise ready queue is empty, but we are guaranteed to have a process in processes[] left
			else {
				curr_time = processes[pointer].start_time;
				seq.push_back(0);
				continue;
			}			
		}
	
		// execute one burst on cpu
		timeOnCPU++;
		Bursts[cpuIndex]--;
		curr_time++;

	


	} // end of main loop




}




// 






























/*
 
	// Used to test how we are given each process
    for (Process &p : processes) {
        printf("\npid: %d  arrival: %ld    burst: %ld\n", p.id, p.arrival_time, p.burst);
        p.finish_time = p.arrival_time + p.burst;
        p.start_time = p.arrival_time;

        seq.push_back(p.id);
    }
    


	// Test if ready queue is filled up with indexes of all processes
    printf("\n[");
    for (int n : readyQueueIndexes) {
        printf("%d, ", n);
    }
    printf("]\n");


*/



