// program takes a directory name as a command line argument and lists all the entry name and their sizes.
// If an entry is a directory, then program recursively enters that directory and lists it's entries as well
// We also keep track of the failed attempts to add the fileSize and fileName into the directory
// *** This program uses a while loop instead of recursion


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>



// global variables (can be accessed by any function)
std::vector<std::pair<int64_t, std::string>> sizeAndFileName;    // vector stores pairs of type (int, string)


// This function takes a file name and retrieves the stats of this file using stat()
bool getStats(std::string fileName) {
    // this struct will store info about the file
    struct stat filestat;

    // get information about the file using stat()
    if (stat(fileName.c_str(), &filestat) == 0) {    // convert string to c_string, give the address of filestat
        int64_t size = filestat.st_size;
        sizeAndFileName.push_back(std::make_pair(size, fileName));
        return true;
    }

	// print error message if stat fails
    else {
		printf("stat failed when trying to open %s\n", fileName.c_str());
        return false;
    }
}





// takes name of directory and recursively traverses each entry of the directory
bool processDir(std::string dirName) {
	
	// create vector to store directories that we must traverse
	std::vector<std::string> stack;

	// add the directory specified in the command line to the stack
	stack.push_back(dirName);

	// some variables we will need inside the loop:
	std::string aDirName;
	DIR *dr;							      // pointer used to store result of opendir()
	struct dirent *de;                        // pointer used to sotre result of readdir()
	std::string entryName;					  // used to hold name of each directory entry
	bool status;	


	// In each loop, we will traverse a directory in the stack, adding it's subdirectories to the stack
    // 		and obtaining the name and size of it's files.
	// We will only stop looping once the stack becomes empty
	while (!stack.empty()) {
		// remove directory name from the stack
		aDirName = stack.back();
		stack.pop_back();

		// open the directory
		dr = opendir(aDirName.c_str());
		
		// check if directory was opened
		if (dr == NULL) {
        	printf("could not open directory %s\n", aDirName.c_str());
    	    return false;
	    }


		// read the directory contents
		while((de = readdir(dr)) != NULL) {
			entryName = de->d_name;   // no need to convert it to c_string
			if ((entryName == ".") || (entryName == "..")) continue;       // go to next iteration if directory entry is "." or ".."

			// If directory entry itself is a directory, add directory entry to stack
			if (de->d_type == DT_DIR) {
				entryName = aDirName + "/" + entryName;
				stack.push_back(entryName);
			}

			// If directory entry is a file, then add file name and size to our vector
			else { 
				status = getStats(aDirName + "/" + entryName);
				if (status == false) return false;
			}
			
		}
		// make sure to close each directory
		closedir(dr);
	}
	
	return true;
}


int main(int argc, char* argv[]) {
    // check if number of command line arguments are correct
    if ((argc) != 2) {
        printf("Run the program with a directory name as a command line argument\n");
        return 1;
    }


    // use processDir to get each directory entry and size and add it to vector sizeAndFileName
    processDir(std::string(argv[1]));

    // sort the vector sizeAndFileName by size of the first item of each pair
    sort(sizeAndFileName.begin(), sizeAndFileName.end());


	// Print number of (stat) fails
    printf("Number stat fails: 0\n---------\n");


    // Go through the sorted sizeAndFileName vector and print out names/sizes
    for(auto pair : sizeAndFileName){
        printf("File: %s\n\tSize: %ld\n---------\n", pair.second.c_str(), pair.first);
    }


    return 0;

}

