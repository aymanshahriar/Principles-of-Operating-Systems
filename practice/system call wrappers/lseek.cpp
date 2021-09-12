#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
using namespace std;



int main(int argc, char* argv[]) {

	int fd = open(argv[1], O_RDONLY);
	
	char c;
	int offset = 1;
	while (read(fd, &c, 1) > 0) {
		cout << c << endl;
		
		lseek(fd, offset, SEEK_CUR);
		
	}

	
}
