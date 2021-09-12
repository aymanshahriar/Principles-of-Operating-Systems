// you need to modify this file

#include "scheduler.h"
#include "common.h"
#include <queue>

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

	while (processesLeft > 0) {
		// if process in cpu and finished it's bursts, add id to seq, add finish time, set cpu=-1, decrement processesLeft, reset timeSlice
		if ((cpu > -1) && (bursts[cpu] == 0)) {
			seq.push_back(processes[cpu].id);
			processes[cpu].finish_time = currentTime;
			cpu = -1;
			processesLeft--;
			timeSlice = quantum;
			continue;
		}

		// if process in cpu and has exceeded time slice, add id to seq, add index to ready queue, set cpu = -1, reset timeSlice
		if ((cpu > -1) && (timeSlice == 0)) {
			if (!readyQueue.empty()) {
				seq.push_back(processes[cpu].id);
				readyQueue.push(cpu);
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
			readyQueue.push(index);
			index++;
			continue;
		}

		// If cpu is idle and ready queue is not empty, pop a process index from ready queue into cpu
		if ((cpu == -1) && (!readyQueue.empty())) {
			cpu = readyQueue.front();
			readyQueue.pop();
			// if this is the first time process is running, update start time
			if (processes[cpu].start_time == -1) {
				processes[cpu].start_time = currentTime;
			}
			continue;
		}

		// If cpu is not idle, execure one burst, update timeSlice
		if (cpu > -1) {
			bursts[cpu]--;
			timeSlice--;
		}
		// whether cpu is idle or not, update current time
		currentTime++;


	} 

}



//(make sure to add timeSlice-- and bursts[cpu]-- at the end)






