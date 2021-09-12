// The stat() system call wrapper takes a fileName and a pointer to a stat structure, and fills that stat structure with information about the file.


#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
using namespace std;


int main(int argc, char* argv[]) {

	// exit if not enough arguments
	if (argc < 2) {
		cout << "Not enough arguments\n";
		return 1;
	}


	struct stat filestat;

	if (stat(argv[1], &filestat) == 0) {
		printf("Device Id: %lu\n", filestat.st_dev);
		printf("Inode number: %lu\n", filestat.st_ino);
		printf("Protection: %u\n", filestat.st_mode);
		printf("Device Id (if special file): %lu\n", filestat.st_rdev);
		printf("File size: %ld\n", filestat.st_size);
		/*
		printf(": %d", );
		printf(": %d", );
		printf(": %d", );
		printf(": %d", );
		*/

	}


	else {
		cout << "Something went wrong, stat did not work\n";
	}






}

