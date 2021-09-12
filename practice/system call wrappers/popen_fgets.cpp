#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>


int main() {
	FILE * fp = popen("/bin/ls -a", "r");   // just using ls also works. If using std::string, make sure to convert to c-string first 
 	if (fp == NULL) {
 		fprintf( stderr, "popen failed.\n");
 		exit(-1);
	}

 	char buff[4096];
 	while (fgets(buff, sizeof(buff), fp) != NULL) {	
		printf("%s", buff);
	}
 	pclose(fp);


}
