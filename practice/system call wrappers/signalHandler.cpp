#include <stdio.h> 
#include <stdlib.h> 
#include <signal.h> 
#include <unistd.h>
#include <iostream>
using namespace std;

void sigint_handler(int signum) {
	cout << "\nCaught signal: " << signum << endl;
	cout << "Lol <ctrl-c> will not stop me\n";
}

int main() {
	// catch the signal caused by <ctrl-c>
	signal(SIGINT, sigint_handler);

	
	for (int i = 0; i <= 10; i++) {
		cout << "Loop " << i << endl;
		sleep(1);
	}

	return 0;
}
