// We will use system call system() to execute a terminal command


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;


int main() {
	cout << "before ls \n";
	
	system("/bin/ls -a");

	cout << "after ls \n";



	return 0;
}








