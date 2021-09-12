/// =========================================================================
/// Written by pfederl@ucalgary.ca in 2020, for CPSC457.
/// =========================================================================
/// You need to edit this file.
///
/// You can delete all contents of this file and start from scratch if
/// you wish, but you need to implement the getDirStats() function as
/// defined in "getDirStats.h".

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


// global variables (can be accessed by any function)
std::vector<std::pair<long, std::string>> sizeAndFileName;    // vector stores pairs of type (int, string)
//std::vector<std::string> fileTypes;                            // vector to store file types
std::unordered_map<std::string, long> umap;
long numDirectories = 0;                                      // keeps track of all subdirectories encountered



static bool is_dir( const std::string & path) {
	struct stat buff;
	if( 0 != stat( path.c_str(), & buff)) return false;
	return S_ISDIR(buff.st_mode);
}


// used in sorting vectors of pairs by their second members
bool sortBySecond(const std::pair<std::string,long> &pair1, const std::pair<std::string,long> &pair2) { 
	if (pair1.second == pair2.second) {
		return (pair1.first > pair2.first);
	}
    return (pair1.second > pair2.second); 
} 


/* This function takes a file name and retrieves the stats of this file using stat() */
bool getStats(std::string fileName) {
    // this struct will store info about the file
    struct stat filestat;

    // get information about the file using stat()
    if (stat(fileName.c_str(), &filestat) == 0) {    // convert string to c_string, give the address of filestat
        long size = filestat.st_size;
        sizeAndFileName.push_back(std::make_pair(size, fileName));
        return true;
    }

    // print error message if stat fails
    else {
        printf("stat failed when trying to open %s\n", fileName.c_str());
        return false;
    }
}



/* Takes name of directory and recursively traverses each entry of the directory, adding the name and sizes of files into sizeAndFileName */
bool processDir(std::string dirName) {

    // create vector to store directories that we must traverse
    std::vector<std::string> stack;

    // add the directory specified in the command line to the stack
    stack.push_back(dirName);

    // some variables we will need inside the loop:
    std::string aDirName;
    DIR *dr;                                  // pointer used to store result of opendir()
    struct dirent *de;                        // pointer used to sotre result of readdir()
    std::string entryName;                    // used to hold name of each directory entry
    bool status;


    // In each loop, we will traverse a directory in the stack, adding it's subdirectories to the stack
    //      and obtaining the name and size of it's files.
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

			entryName = aDirName + "/" + entryName;
            
			// If directory entry itself is a directory, add directory entry to stack
            if (is_dir(entryName)) {    
                stack.push_back(entryName);
                numDirectories++;
            }

            // If directory entry is a file, then add file name and size to our vector
            else {
                status = getStats(entryName);
                if (status == false) return false;
            }
        }
        // make sure to close each directory
        closedir(dr);
    }
    return true;
}


/*
bool getCommonTypes() {

	// initialize an unordered map
	unordered_map<std::string, long> umap;
	
	for (auto pair : sizeAndFileName) {
		// If name does not exist as a key in our umap, then create an entry with name as key and zero as value
		if (umap.find(pair.second) == umap.end()) {
			umap[pair.second] = 0;
		}

		// If name already exists as a key, increment value that corresponds to name by 1
		else {
			umap[pair.second]++;
		}

	}

}
*/

bool getCommonTypes() {
	std::string command;
	FILE * fp;					// used in popen	  
	char buff[4096];			// used in fgets
	std::string result;
	std::string fileType = "";

	// For each file name, use popen to execute command "file -b <filename>" to get type of each file
	// use fgets to retrieve the result
	for (auto pair : sizeAndFileName) {
		command = "file -b " + pair.second;
		fp = popen(command.c_str(), "r");
		
		// check if popen worked
		if (fp == NULL) {
			printf("popen did not work\n");
			printf("%s\n", pair.second.c_str());
			return false;
		}

		if (fgets(buff, 4096, fp) == NULL) {
			printf("fgets did not work");
			return false;
		}
		pclose(fp);
		result = buff;

		// Keep adding characters of type into mainType, until you encounter a comma or newline
		fileType = "";       // makes sure fileType gets reset after each loop
		for (unsigned long i = 0; i < result.length(); i++) {
			if ((result[i] == ',') || (result[i] == '\n')) break;
			fileType += result[i];
		}
		
		// If file type does not exist as a key in our umap, then create an entry with type as key and zero as value
		// Else if file type already exists as key, increment value that correspont to that type by 1
        if (umap.find(fileType) == umap.end()) umap[fileType] = 1;
      
        else umap[fileType]++;
        
		// now that umap is filled with file types and their occurences, sort umap by occurences

	}	// end of large for loop
	
	return true;
}





//
// getDirStats() computes stats about directory dir_name
// if successful, it return true and stores the results in 'res'
// on failure, it returns false, and res is in undefined state
//

bool getDirStats(const std::string & dir_name, Results & res) {

	if( ! is_dir(dir_name)) return false;

	// call processDir function to fill up sizeAndFileName
	if (processDir(dir_name) == false) return false;

	// sort the vector sizeAndFileName by size of the first item of each pair (so sorting by file size)
    std::sort(sizeAndFileName.begin(), sizeAndFileName.end());

	/* a), b) Get name and size of largest file */
	std::pair<long, std::string> largestPair = sizeAndFileName.back();

	res.largest_file_path = largestPair.second;
	res.largest_file_size = largestPair.first;

	/*c) Get total number of files in the directory*/
	/*e) Get total file size of all the files*/
	long numFiles = 0;
	long totalSize = 0;
	for (auto pair : sizeAndFileName) {
		numFiles++;
		totalSize += pair.first;
	}
	res.n_files = numFiles;
	res.all_files_size = totalSize;

	/* d) Get total number for subdirectories in the directory*/
    res.n_dirs = numDirectories;

	
	/* e) Get the most common types from umap */
	if (getCommonTypes() == false) {
		printf("commonTypes did not work");
		return false;
	}
	
	// convert unordered map to vector of pairs, then we will be able to sort it
	std::vector<std::pair<std::string, long>> typeVec = std::vector<std::pair<std::string, long>>(umap.begin(), umap.end());
	std::sort(typeVec.begin(), typeVec.end(), sortBySecond);

	// fill in res.most_common_types
	std::vector<std::string> mostCommonTypes;
	for (unsigned long i = 0; i < 5; i++) {
		if (i == typeVec.size()) break;
		mostCommonTypes.push_back(typeVec[i].first);    //+ "  " + std::to_string(typeVec[i].second));
	}

	res.most_common_types = mostCommonTypes;

    //////////////////////////////////////////////////////////
	/*
    res.most_common_types = {
    "C source, ASCII text",
    "makefile script, ASCII text",
    "C++ source, ASCII text",
    "directory"
    };
	*/
    std::vector<std::string> group1;
    group1.push_back(dir_name + "/file1.cpp");
    group1.push_back(dir_name + "/lib/sub/other.c");
    res.duplicate_files.push_back(group1);
    std::vector<std::string> group2;
    group2.push_back(dir_name + "/readme.md");
    group2.push_back(dir_name + "/docs/readme.txt");
    group2.push_back(dir_name + "/x.y");
    res.duplicate_files.push_back(group2);



	return true;
}






