// we will read from a file using the read wrapper used in assign 1

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

#define BUFFERSIZE 100000

int main(int argc, char* argv[]) {
	
	// obtain file descriptor of the file we want to read from
	// If we want to read from stdin (ie, execute the program as:  ./read << "text text text"), then we use fd = 0
	int fd = open(argv[1], O_RDONLY);   // the access flag indicates that we are opening this file for reading only
	
	char buffer[BUFFERSIZE];

	read(fd, buffer, BUFFERSIZE);     // read BUFFERSIZE bytes into buffer
	
	for (int i = 0; i < BUFFERSIZE; i++) {
		if (buffer[i] == '\0'){    // ie, if we encounter the null value, then that's the end of the text in the file. 
			break;                 // the above is the same as (buffer[i] == 0)
		}
		

		std::cout << buffer[i];		
			
	}
	std::cout << "\n";

}
