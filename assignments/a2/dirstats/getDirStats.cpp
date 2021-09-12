/// =========================================================================
/// Written by pfederl@ucalgary.ca in 2020, for CPSC457.
/// =========================================================================
/// 
/// CPSC 457 Assignment 4
/// Name: Ayman Shahriar    UCID:10180260
/// Tutorial: T03
/// 
/// 

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
std::unordered_map<std::string, long> umap;
std::unordered_map<std::string, std::vector<std::string>> groupsMap;
long numFiles = 0;
long totalFileSize = 0;
long numDirectories = 0;                                      // keeps track of all subdirectories encountered
std::string largestFileName = "";
long largestFileSize = 0;


static bool is_dir( const std::string & path) {
	struct stat buff;
	if( 0 != stat( path.c_str(), & buff)) return false;
	return S_ISDIR(buff.st_mode);
}


/* Takes name of directory and recursively traverses each entry of the directory, adding the name and sizes of files into sizeAndFileName */
bool processDir(std::string dirName) {

    // some variables we will need inside the loop:
    std::string aDirName;
    DIR *dr;                                  // pointer used to store result of opendir()
    struct dirent *de;                        // pointer used to sotre result of readdir()
    std::string entryName;                    // used to hold name of each directory entry

    struct stat filestat;                    // this struct will store info about the file stats
	long size;                               // stores size of file
	std::string digest;                      // stores sha256 digest of each file

	// variables used when getting the type of a file using popen() and fgets()
	std::string command;
    FILE * fp;                  // used in popen      
    char buff[4096];            // used in fgets
    std::string result;
    std::string fileType = "";


    // create vector to store directories that we must traverse
    std::vector<std::string> stack;

    // add the directory specified in the command line to the stack
    stack.push_back(dirName);


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
        if (dr == NULL) return false;
        
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

            // If directory entry is a file, then add sha256 digest to groupMap, and find file size using stat()
            else {
       	
			//////////////////////////////////// Get sha256 digest Of fileName, add to groupsMap  //////////////////////////////////////////////////

        		digest = sha256_from_file(entryName);
				if (digest == "") return false;
				
    			// we do not even have to check if digest exists as a key. If it exists, the statement adds entryName to the existing vector
    			// If the digest doesnt exist as key, the statement creates an entry with key = digest and value = a vector that stores entryName
            	groupsMap[digest].push_back(entryName);
        	
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			//////////////////////////////////// get size of the file using stat() /////////////////////////////////////////////////////////////////

			    if (stat(entryName.c_str(), &filestat) != 0) {    // convert string to c_string, give the address of filestat
                    return false;
				}
        		size = filestat.st_size;
        		numFiles++;
        		totalFileSize += size;
					
				// check if the file is the largest discovered file
				if (filestat.st_size > largestFileSize) {
					largestFileName = entryName;
					largestFileSize = filestat.st_size;
				}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			/////////////////////////////////// Get the type of the file using popen() and fgets() ///////////////////////////////////////////////

    			// For each file name, use popen to execute command "file -b <filename>" to get type of each file
    			// use fgets to retrieve the result
    
        		command = "file -b " + entryName;
        		fp = popen(command.c_str(), "r");

        		// check if popen worked
        		if (fp == NULL) return false;

        		if (fgets(buff, 4096, fp) == NULL) return false;
        	
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
      
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        	
			}
        }
        // make sure to close each directory
        closedir(dr);
    }
    return true;
}




//
// getDirStats() computes stats about directory dir_name
// if successful, it return true and stores the results in 'res'
// on failure, it returns false, and res is in undefined state
//

bool getDirStats(const std::string & dir_name, Results & res) {
	if( ! is_dir(dir_name)) return false;

	// call processDir function to fill up all the global variables
	if (processDir(dir_name) == false) return false;
	
	/* a), b) Get name and size of largest file */
		// if there are no files, return "" for name and -1 for size
	if (numFiles == 0) {
		res.largest_file_size = -1;
	}
	else {
		res.largest_file_path = largestFileName;
		res.largest_file_size = largestFileSize;
	}

	/*c) Get total number of files in the directory*/
	/*e) Get total file size of all the files*/
	res.n_files = numFiles;
	res.all_files_size = totalFileSize;

	/* d) Get total number for subdirectories in the directory*/
    res.n_dirs = numDirectories;



	/* e) Get the most common types from umap */
	
	// convert unordered map to vector of pairs, then we will be able to sort it
	std::vector<std::pair<long, std::string>> typeVec;
    for(auto &pair : umap) {
		// We put the number of occurence as first in pair so that it will be easier to sort
		// We negate the number of occurence so that we can sort it easier
        typeVec.emplace_back(-pair.second, pair.first);
	}
	
	// partially sort the first 5 pairs
	if(typeVec.size() > 5) {
    	std::partial_sort(typeVec.begin(), typeVec.begin() + 5, typeVec.end());
    	typeVec.resize(5); // We do not need all entries after the first 5
  	} 
	else std::sort(typeVec.begin(), typeVec.end());
  


	// fill in res.most_common_types
	std::vector<std::string> mostCommonTypes;
	for (auto pair : typeVec) {
		res.most_common_types.push_back(pair.second);   
	}


	/* f) Get 5 largest groups of duplicate files */
	std::vector<std::pair<long, std::vector<std::string>>> groupsVec;
  	for(auto & pair : groupsMap) {    
		// We do not need the digest at all (so ignore pair.first)
		// We put the size of the vector as the first in pair of gropusVec so that it will be easier to sort
		// We negate the size of the vector that we can sort it easier
    	groupsVec.emplace_back(-pair.second.size(), pair.second);
	}
	
	// Partially sort the first 5 pairs	
	if (groupsVec.size() > 5) {
		std::partial_sort(groupsVec.begin(), groupsVec.begin() + 5, groupsVec.end());
		groupsVec.resize(5); // We do not need all entries after the first 5
	}
	else {
		std::sort(groupsVec.begin(), groupsVec.end());
	}
	
	// fill in res.most_common types, but just skip that if groupsVec is empty
	if (groupsVec.empty()) return true;
	for (auto &pair : groupsVec) {
		if (pair.first == -1) break;       
		res.duplicate_files.push_back(pair.second);
	}
	

    //////////////////////////////////////////////////////////
/*	
    res.most_common_types = {
    "C source, ASCII text",
    "makefile script, ASCII text",
    "C++ source, ASCII text",
    "directory"
    };
	
	
	
    std::vector<std::string> group1;
    group1.push_back(dir_name + "/file1.cpp");
    group1.push_back(dir_name + "/lib/sub/other.c");
    res.duplicate_files.push_back(group1);
    std::vector<std::string> group2;
    group2.push_back(dir_name + "/readme.md");
    group2.push_back(dir_name + "/docs/readme.txt");
    group2.push_back(dir_name + "/x.y");
    res.duplicate_files.push_back(group2);
	
*/

	return true;
}






