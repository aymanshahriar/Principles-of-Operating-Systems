//
//	Assignment 5
//
//	Author: Ayman Shahriar    ID: 10180260
//	Tutorial: T03      
//
//	Citation (APA format for lecture notes):
//	Federl, P. (2020). C++ implementation of the pseudocode given as hints in the document "A5-F20" 
//      [Example program].  Retrieved from https://d2l.ucalgary.ca/d2l/le/content/327860/Home
//                                         
//
//


#include "memsim.h"
#include <cstdio>
#include <list>
#include <set>
#include <unordered_map>



// Represents a single memory partition
struct Partition {
    long tag ;
    long size ;
    int64_t addr ;

};

// An iterator(pointer) to a list of partitions
typedef std::list<Partition>::iterator PartitionRef;

// used to order set of empty partitions
struct scmp {
	bool operator()( const PartitionRef & c1, const PartitionRef & c2) const {
	if( c1->size == c2->size)
		return c1->addr < c2->addr;
	else
		return c1->size < c2->size;
	}
};



// Implements the allocate, deallocate, and getStats functions, and contains variables and data structures used to implement them
struct Simulator {

	int64_t page_size;
	int64_t max_free_partition_size = 0;
	int64_t n_pages_requested = 0;

    // constructor
    Simulator(int64_t page_size_) { page_size = page_size_; }

	// all partitions (to get std::prev() and std::next() nodes)
	std::list<Partition> partitions;

	// pointers to free partitions, sorted by size/address
	std::set<PartitionRef,scmp> free_blocks;

	// Map that contains all occupied partitions (for quick access to all tagged partitions)
	std::unordered_map<long, std::vector<PartitionRef>> tagged_blocks;

	// helper variables for allocate
	PartitionRef emptyPartitionPtr;
	PartitionRef lastPartitionPtr;
	long numPages;
	long lastPartitionSize;
	Partition emptyPartition;
	Partition newPartition;
	Partition newNode;

	// allocate function, used to allocate memory
    // Input parameters:
    //     tag: the tag of the process to be inserted
    //     size: the size of the process to be inserted
    // Output patameters:
    //     none, the function modifies the processes, free_blocks and tagged_blocks
  	void allocate(int tag, int size) { 
		std::list<Partition> dummy = {{-1, size, 0}};
		auto sbesti = free_blocks.lower_bound( dummy.begin());
 		emptyPartitionPtr = partitions.end();
 		
		// if there is a partition big enough for request:
		if( sbesti != free_blocks.end()) {
 			emptyPartitionPtr = *sbesti;

			ptrToInsert = std::next(emptyPartitionPtr);
			emptyNode = {-1, (emptyPartitionPtr->size)-size, (emptyPartitionPtr->addr)+size};
			newNode = {tag, size, emptyPartitionPtr->addr};
			free_blocks.erase(emptyPartitionPtr);
			partitions.erase(emptyPartitionPtr);
			
			// add new node to partitions and tagged blocks
			partitions.insert(ptrToInsert, newNode);
			tagged_blocks[tag].push_back(std::prev(ptrToInsert));

			// if additional space remaining, add to partitions and free_blocks
			if (emptyNode.size > 0) {
				partitions.insert(ptrToInsert, emptyNode);
				ptrToInsert--;
				free_blocks.insert(ptrToInsert);
			}
		}

		// if there isn't a partition big enough for request:
 		if( emptyPartitionPtr == partitions.end()) {
			// calculate number of pages to allocate:
			
			// If last partition is free, calculate space needed
			if (std::prev(partitions.end())->tag == -1) {
							
				// calculate pages needed
				numPages = ((size-(std::prev(partitions.end())->size))/page_size) + (((size-(std::prev(partitions.end())->size))%page_size) != 0);

				ptrToInsert = partitions.end();
				emptyNode = {-1, (numPages*page_size)+(std::prev(partitions.end())->size)-size, (std::prev(partitions.end())->addr)+size};
				newNode = {tag, size, std::prev(partitions.end())->addr};
				free_blocks.erase(std::prev(partitions.end()));
				partitions.erase(std::prev(partitions.end()));

				// add new node to partitions and tagged blocks
				partitions.insert(ptrToInsert, newNode);
				tagged_blocks[tag].push_back(std::prev(ptrToInsert));

				// if additional space remaining, add to partitions and free_blocks
				if (emptyNode.size > 0) {
					partitions.insert(ptrToInsert, emptyNode);
					ptrToInsert--;
					free_blocks.insert(ptrToInsert);
				}			
			}

			// Otherwise last partition is not empty, calculate new space needed
			else {
				//calculate pages needed
				numPages = (size / page_size) + ((size % page_size) != 0);
				
				ptrToInsert = partitions.end();
				emptyNode = {-1, (numPages*page_size)-size, ((std::prev(partitions.end())->size) + (std::prev(partitions.end())->addr) + size)};
				newNode = {tag, size, ((std::prev(partitions.end())->size) + (std::prev(partitions.end())->addr))};
				
				// add new to partitions and tagged_blocks
				partitions.insert(ptrToInsert, newNode);
				tagged_blocks[tag].push_back(std::prev(ptrToInsert));

				// if additional space remaining, add partitions to free_blocks
				if (emptyNode.size > 0) {
					partitions.insert(ptrToInsert, emptyNode);
					ptrToInsert--;
					free_blocks.insert(ptrToInsert);
				}

			}
		
			// Add the pages requested to total pages requested
			n_pages_requested += numPages;
		}
	}



	// helper variables for deallocate
	std::vector<PartitionRef> taggedArr;
	Partition emptyNode;
	PartitionRef ptrToInsert;

	// deallocate function, used to deallocate memory
	// Input parameters:
    //     tag: the tag used by the process to be deallocated
    // Output patameters:
    //     none, the function modifies the processes, free_blocks and tagged_blocks
  	void deallocate(int tag) { 
		
		// if tag belongs in tagged_blocks:
		if (tagged_blocks.find(tag) != tagged_blocks.end()) {
			taggedArr = tagged_blocks[tag];

			// erase vector in tagged_blocks with key = tag
			tagged_blocks.erase(tag);
	
			// process each block that have the specified tag
			for (PartitionRef partitionPointer : taggedArr) {
				partitionPointer->tag = -1;

				// this is when both sides of the deallocated partition are free
				if ((partitionPointer != partitions.begin()) && (partitionPointer != partitions.end()) && (std::prev(partitionPointer)->tag == -1) && (std::next(partitionPointer)->tag == -1)) {
					
					// emptyNode-merged node to be inserted   prtToInsert: location in partitions to insert
					ptrToInsert = std::next(std::next(partitionPointer));
					emptyNode = {-1, (partitionPointer->size)+(std::prev(partitionPointer)->size)+(std::next(partitionPointer)->size), std::prev(partitionPointer)->addr};
					// remove the three nodes
					free_blocks.erase(std::prev(partitionPointer));
					free_blocks.erase(std::next(partitionPointer));
					partitions.erase(std::prev(partitionPointer));
					partitions.erase(std::next(partitionPointer));
					partitions.erase(partitionPointer);
				
					// insert new node
					partitions.insert(ptrToInsert, emptyNode);
					--ptrToInsert;
					free_blocks.insert(ptrToInsert);
				}

				// this is when the left side of the deallocated partition is free
				else if ((partitionPointer != partitions.begin()) && (std::prev(partitionPointer)->tag == -1)) {
					// emptyNode: merged node to be inserted  ptrToInsert: location in partitions to insert
					ptrToInsert = std::next(partitionPointer);
					emptyNode = {-1, (partitionPointer->size)+(std::prev(partitionPointer)->size), std::prev(partitionPointer)->addr};
					
					// remove the two nodes
					free_blocks.erase(std::prev(partitionPointer));
					partitions.erase(std::prev(partitionPointer));
					partitions.erase(partitionPointer);
					
					// insert new node
					partitions.insert(ptrToInsert, emptyNode);
					--ptrToInsert;
                    free_blocks.insert(ptrToInsert);

				}

				// this is when the right side of the deallocated partition is free
				else if ((partitionPointer != partitions.end()) && (std::next(partitionPointer)->tag == -1)) {
					// emptyNode: merged node to be inserted  ptrToInsert: location in partitions to insert
					ptrToInsert = std::next(std::next(partitionPointer));
					emptyNode = {-1, (partitionPointer->size)+(std::next(partitionPointer)->size), partitionPointer->addr};
					
					// remove the two nodes
					free_blocks.erase(std::next(partitionPointer));
					partitions.erase(std::next(partitionPointer));
					partitions.erase(partitionPointer);

					// insert new node
					partitions.insert(ptrToInsert, emptyNode);
					--ptrToInsert;
                    free_blocks.insert(ptrToInsert);
				}

				// this is when no sides of the deallocated partition is free
				else {
					// no need to remove partitionsPtr from partitions, but we need to insert it into free_blocks
					partitionPointer->tag = -1;
					free_blocks.insert(partitionPointer);

				}				
			}
		}
	}


	// This function will return the number of pages requested and the largest free partition size
    // Input parameters:
    //     result: struct that contains the attributes max_free_partition_size and n_pages_requested		 
    // Output parameters:
    //     result: fill it's attributes
  	void getStats(MemSimResult & result) {
    	// return the size of the maximum free partition (set to 0 if no free partitions exist)
    	result.max_free_partition_size = (!free_blocks.empty()) ? (*std::prev(free_blocks.end()))->size : 0;
    	// return the total number of pages requested
    	result.n_pages_requested = n_pages_requested;
  	}

};





// ===================================
// input parameters:
//    page_size: integer in range [1..1,000,000]
//    requests: array of requests, each with tag and size
// output parameters:
//    result: populate with correct values before returning
void mem_sim(int64_t page_size, const std::vector<Request> & requests, MemSimResult & result) {
	Simulator sim(page_size);
	// add an empty partition of size 0
	sim.partitions.push_back({-1, 0, 0});
	sim.free_blocks.insert(sim.partitions.begin());

  	for (const auto & req : requests) {
    	if (req.tag < 0) {
      		sim.deallocate(-req.tag);
	  	} else {
      		sim.allocate(req.tag, req.size);
   		}
  	}
  	sim.getStats(result);	
}









/* Some notes on Bidirectional Iterators: */

// Basically, an iterator is a pointer to an object in a container
// And if you increment the iterator, it will point to the next object in the container
// container.begin() - points to the first element     container.end() - points past the last element
// To access the object pointed to by the iterator, you use "*"  (just like with a pointer)
// If the iterator points to a Struct, then use iteratorName->attr to access an attribute of the struct
// One of the differences between an iterator and a pointer: if you increment an iterator by 1, it will point to the next element
//     in the container. If you increment a pointer by 1, I don't think it will point to the next element in the container
// If you decrement an iterator beyond container.begin(), it will go to the last element


