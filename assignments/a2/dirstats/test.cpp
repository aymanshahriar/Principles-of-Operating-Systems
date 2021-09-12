#include "getDirStats.h"
#include "digester.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <dirent.h>
#include <algorithm>
#include <stdio.h>
#include <unordered_map>


std::vector<std::pair<std::string, long>> typeVec;


int main(int argc, char* argv[]) {

	std::string command;
    FILE * fp;                  // used in popen      
    char buff[8192];            // used in fgets
    std::string result;
    std::string fileType;
    std::string numType;        // later convert to long

	std::string s = argv[1];
	command = "find " + s + " -type f | xargs file -b | awk -F, '{print $1}' | sort | uniq -c | sort -nr";
	
	fp = popen(command.c_str(), "r");

  	// check if popen worked
   	if (fp == NULL) {
   		printf("popen did not work\n");
       	return false;
  	}
	


	
	while (fgets(buff, 8192, fp) != NULL) {
    	result = buff;
        fileType = "";       // makes sure fileType gets reset after each loop
		numType = "";

		// remove leading whitespaces
        while(isspace(result.front())) {
			result = result.substr(1);
		}
				     		
		// get the number                
		while((result.front() <= '9') && (result.front() >= '0')) {
			numType += result.front();
			result = result.substr(1);
		}		

		// get rid of the space seperating number and type
		result = result.substr(1);
		
		// get rid of the newline at the end
		if (result.back() == '\n') {
			result = result.substr(0, result.find_last_not_of('\n')+1);
		}

		// since everything is aggregated and sorted, no need for a umap, just use a vector of pairs
		typeVec.emplace_back(result, stol(numType));
  	}
	pclose(fp);
	
	for (auto &pair : typeVec) {
		printf("%s----%ld\n", pair.first.c_str(), pair.second);
	}	
	
	
	return 0;
}
