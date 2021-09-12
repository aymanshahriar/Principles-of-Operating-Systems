// 
// Assignment 5
//
// Author: Ayman Shahriar
// 
// ID: 10180260   TUT: T02
//
//


#include "fatsim.h"
#include <cstdio>
#include <iostream>
#include <algorithm>    // std::find



/* Takes a FAT table, an returns longest file block and number of unused blocks
 * Input: FAT table in the form of vector<long>
 *        longest_file_blocks, used to return the length of the longest file blocks
 *        unused_blocks, used to return the number of unused blocks
 *
 * Output: Return the longest file blocks and number of unused blocks in the two
 *         corresponding parameters
*/
void fat_sim(const std::vector<long> & fat, long & longest_file_blocks, long & unused_blocks) {

	// Reset the two return parameters
	longest_file_blocks = 0;
  	unused_blocks = 0;

	// 0: This node has not been visited yet
	// >0: This node has been visited, stores length of node
	// -1: This node has been visited and is part of a cycle
	int visited_length[fat.size()] = {};
	
	
	std::vector<long> stack;
	long neighbour;
	
	for (long node = 0; node < (long)fat.size(); node++) {
		stack.clear();
		if (visited_length[node] == 0) {
			stack.push_back(node);			

			while (!stack.empty()) {			
				neighbour = fat[stack.back()];
				// if current node has neighbour of -1, set it's size to 1, set node to visited, remove from stack
				if (neighbour == -1) {
					visited_length[stack.back()] = 1;
					stack.pop_back();
					continue;
				}
				
				// otherwise we have to find out the details of the neighbour node
				else {
					// if current node has neighbour that is already in stack, then mark visiting node as being in cycle
					if(std::find(stack.begin(), stack.end(), neighbour) != stack.end()) {
    					visited_length[stack.back()] = -1;
						stack.pop_back();
						continue;						
	
					} else {
						// if neighbour of current node has been visited before, then it's length is defined
						if (visited_length[neighbour] != 0) {
							if (visited_length[neighbour] == -1) visited_length[stack.back()] = -1;
							else visited_length[stack.back()] = visited_length[neighbour]+1;
		                    stack.pop_back();
							continue;
					
						// else, go to the visit the neighbour to find it's size
						} else {
							stack.push_back(neighbour);
						}					
					}
				}

			} // end of while loop
		}
	} // end of for loop
	
	

	// calculate longest length, number of free blocks (ie, the blocks that are part of a cycle)
	for (auto size : visited_length) {
		if (size > longest_file_blocks) longest_file_blocks = size;
		if (size == -1) unused_blocks++;
	}


} // end of function









// Some print statements used for testing

    /*
    for(long i=0; i < (long) fat.size(); i++)
        std::cout << length[i] << ' ';
    printf("\n");

    for(long i=0; i < (long) fat.size(); i++)
        std::cout << visited[i] << ' ';
    printf("\n");
    */

	//printf("\n\n\n%ld     %ld\n\n\n", stack.back(), fat[stack.back()]);
	//printf("\n\n\ncont1\n\n\n");
	//printf("\n\n\ncont2\n\n\n");
	//printf("\n\n\ncont3\n\n\n");
	//printf("\n\n\ncont4\n\n\n");
