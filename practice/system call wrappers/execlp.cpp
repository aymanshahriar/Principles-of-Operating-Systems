// we will use system call execlp() to execute an external program

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

using namespace std;

int main() {

	pid_t pid = fork();

	// check if fork was unsuccessful
	if (pid <= -1) {
		cout << "Fork failed";
		exit(-1);
	}


	// if process is child process, then use execlp() system call to execute external program
	else if (pid == 0) {
		execlp("/bin/ls", "ls", "-a", NULL);     // replace child process with ls
		
		cout << "this should never print" << endl;
	}

	
	// if process is parent process, then wait for child process to finish
	else {
		cout << "waiting for child process: " << pid << endl;
	
		while(wait(NULL) > 0);      // while(wait(NULL)) should also work
		
		cout << "child finished\n";
		exit(0);

	}	







	return 0;
}
