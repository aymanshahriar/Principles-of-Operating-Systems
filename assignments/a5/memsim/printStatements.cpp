// this is the file you need to edit

#include "memsim.h"
#include <cstdio>
#include <list>
#include <set>
#include <unordered_map>


// Basically, an iterator is a pointer to an object in a container
// And if you increment the iterator, it will point to the next object in the container
// container.begin() - points to the first element     container.end() - points past the last element
// To access the object pointed to by the iterator, you use "*"  (just like with a pointer)
// If the iterator points to a Struct, then use iteratorName->attr to access an attribute of the struct
// One of the differences between an iterator and a pointer: if you increment an iterator by 1, it will point to the next element
//     in the container. If you increment a pointer by 1, I don't think it will point to the next element in the container
// If you decrement an iterator beyond container.begin(), it will go to the last element



// Represents a single memory partition
struct Partition {
    long tag ;
    long size ;
    int64_t addr ;

	// constructor
//	Partition(long t, long s, int64_t a) { tag = t; size = s; addr = a; }
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



// feel free to use this class as a starting point
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
	std::list<Partition> dummy = {{-1, 0, 0}};

  	void allocate(int tag, int size) { 
		printf("allocating tag=%d size=%d\n", tag, size); 
		dummy.begin()->size = size;
		auto sbesti = free_blocks.lower_bound( dummy.begin());
 		emptyPartitionPtr = partitions.end();
 
		// if there is a partition big enough for request:
		if( sbesti != free_blocks.end()) {
 			emptyPartitionPtr = *sbesti;
		}

		// test
		if (free_blocks.find(partitions.begin()) != free_blocks.end()) printf("the second block is safe 1\n");

		// if there isn't a partition big enough for request:
 		if( emptyPartitionPtr == partitions.end()) {
			// calculate number of pages to allocate:
			
			// Get pointer to last partition in partitions list
			lastPartitionPtr = std::prev(partitions.end());
			// Check if last partition is free, then increase capacity of last partition:
			if (lastPartitionPtr->tag == -1) {
				lastPartitionSize = lastPartitionPtr->size;
				numPages = ((size-lastPartitionSize)/page_size) + (((size-lastPartitionSize)%page_size) != 0);
				// increase capacity of last partition
				lastPartitionPtr->size += (numPages*page_size);
				emptyPartitionPtr = lastPartitionPtr;
			}

			// Otherwise last partition is not empty, we need to add a new, empty partition at the end of partitions list and in free_blocks
			else {
				numPages = ((long)size/page_size) + (((long)size%page_size) != 0);
				emptyPartition = {-1, (numPages*page_size), (numPages*page_size)+(lastPartitionPtr->addr)};
				// insert empty partition at end of process
				partitions.push_back(emptyPartition);
				// insert pointer to empty partition in free_blocks
				free_blocks.insert(std::prev(partitions.end()));
				// set emptyPartitionPtr to point to this new, empty partition
				emptyPartitionPtr = std::prev(partitions.end());
			}

			// Add the pages requested to total pages requested
			n_pages_requested += numPages;

		}
 		
		// now we have an empty partition that is big enough/best fit, and is in both partitions and free_blocks
		// split the empty partition by creating a new partition, inserting it in partitions list (behind empty partition) and tagged_blocks
		if (emptyPartitionPtr == partitions.end()) printf("\ntrue\n");
		emptyPartitionPtr->size -= size;
		emptyPartitionPtr->addr += size;
		if (emptyPartitionPtr == partitions.end()) printf("\ntrue\n");
		newPartition = {tag, size, emptyPartitionPtr->addr};
		
		// insert new partition in partitions list (behind empty partition), tagged_blocks
		partitions.insert(emptyPartitionPtr, newPartition);
		
		if (tagged_blocks.find(tag) != tagged_blocks.end()) tagged_blocks[tag].push_back(std::prev(emptyPartitionPtr));
		else tagged_blocks[tag] = {std::prev(emptyPartitionPtr)}; 
		
		// update empty partition
		if (free_blocks.find(partitions.begin()) != free_blocks.end()) printf("the second block is safe 2, %ld, %ld\n", emptyPartitionPtr->size, size);
		//emptyPartitionPtr->size -= size;
		if (free_blocks.find(partitions.begin()) != free_blocks.end()) printf("the second block is safe 2\n");
		//emptyPartitionPtr->addr += size;
		
		// if empty partition's size is 0, remove it from free_blocks, partitions 
		if (emptyPartitionPtr->size == 0) {
			free_blocks.erase(emptyPartitionPtr);      // erasing using element value
			partitions.erase(emptyPartitionPtr);        // erasing using iterator
		}
		// now check which partition is the largest partition
       	if (!free_blocks.empty()) max_free_partition_size = (*std::prev(free_blocks.end()))->size;
       	else max_free_partition_size = 0;
		//if (free_blocks.find(partitions.begin()) != free_blocks.end()) printf("the second block is safe 2\n");
		
	}







	// helper variables for deallocate
	std::vector<PartitionRef> taggedArr;
	Partition leftPartition;

  	void deallocate(int tag) { 
		printf("deallocating tag=%d\n", tag); 
		
		// if tag belongs in tagged_blocks:
		if (tagged_blocks.find(tag) != tagged_blocks.end()) {
			taggedArr = tagged_blocks[tag];
			printf("size of taggedArr:%ld\n",taggedArr.size());
/*			for (PartitionRef partitionPointer : taggedArr) {
				printf("tagged arr: |tag=%ld size=%ld addr=%ld|", partitionPointer->tag, partitionPointer->size, partitionPointer->addr);
			}
*/			for (PartitionRef partitionPointer : taggedArr) {
				partitionPointer->tag = -1;
				
				if (partitionPointer != partitions.begin()) {
					if (std::prev(partitionPointer)->tag == -1) {
						// merge the two partitions, update size, address
						leftPartition = *std::prev(partitionPointer);
						partitionPointer->size += leftPartition.size;						
						partitionPointer->addr = leftPartition.addr;
						printf("\nleftPartition tag:%ld size:%ld address:%ld\n", leftPartition.tag, leftPartition.size, leftPartition.addr);
						// merge is done, remove left partition pointer from free_blocks
						printf("size before erase: %ld\n", free_blocks.size());
						if (free_blocks.find(partitions.begin()) != free_blocks.end()) printf("this should print\n");
						free_blocks.erase(std::prev(partitionPointer));    	   // erasing using element value
						printf("size after erase: %ld\n", free_blocks.size());
						// remove left partition from partitions list
						partitions.erase(std::prev(partitionPointer));         // erasing using iterator
					}
				}

				if (partitionPointer != std::prev(partitions.end())) {
					if (std::next(partitionPointer)->tag == -1) {
						printf("\nPartition tag:%ld size:%ld address:%ld\n", partitionPointer->tag, partitionPointer->size, partitionPointer->addr);
						// merge two partitions, update size, (no need to "update" address)
						partitionPointer->size += std::next(partitionPointer)->size;
						// merge done, remove right partition pointer from free_blocks
						free_blocks.erase(std::next(partitionPointer));        // erasing using element value
						// remove right partition from partitions list
						partitions.erase(std::next(partitionPointer));           // erasing using iterator
					}
				}
				// Add the freed (and possible merged) partition's pointer into free_blocks
				free_blocks.insert(partitionPointer);
				
			}

			// remove entry with tag from tagged_blocks
			tagged_blocks.erase(tag);          // erasing by key, as opposed to erasing using iterator
			// now check which partition is the largest partition
			if (!free_blocks.empty()) max_free_partition_size = (*std::prev(free_blocks.end()))->size;
			else max_free_partition_size = 0;
		}
	
	
	}






  	void getStats(MemSimResult & result) {
    	// return the size of the maximum free partition (set to 0 if no free partitions exist)
    	result.max_free_partition_size = max_free_partition_size;
    	// return the total number of pages requested
    	result.n_pages_requested = n_pages_requested;
  	}

  
};

// re-implement the following function
// ===================================
// input parameters:
//    page_size: integer in range [1..1,000,000]
//    requests: array of requests, each with tag and size
// output parameters:
//    result: populate with correct values before returning
void mem_sim(int64_t page_size, const std::vector<Request> & requests, MemSimResult & result) {
	Simulator sim(page_size);
	//printf("\n %ld \n",sim.lastPartitionSize);
	// add an empty partition of size 0
	sim.partitions.push_back({-1, 0, 0});
	sim.free_blocks.insert(sim.partitions.begin());


  	for (const auto & req : requests) {
    	if (req.tag < 0) {
			printf("\n %ld, %ld, %ld \n",sim.partitions.size(), sim.free_blocks.size(), sim.tagged_blocks.size());
      		sim.deallocate(-req.tag);
			for (auto p : sim.partitions) {
				printf("|tag=%ld size=%ld addr=%ld|", p.tag, p.size, p.addr);			
			}printf("\n");
			for (auto a = sim.free_blocks.begin(); a != sim.free_blocks.end(); a++) {
				printf("|tag:%ld size=%ld addr:%ld|", (*a)->tag, (*a)->size, (*a)->addr);
			}
			printf("\n");
			printf("\n %ld, %ld, %ld \n",sim.partitions.size(), sim.free_blocks.size(), sim.tagged_blocks.size());
    	} else {
			printf("\n %ld, %ld, %ld \n",sim.partitions.size(), sim.free_blocks.size(), sim.tagged_blocks.size());
      		sim.allocate(req.tag, req.size);
			for (auto p : sim.partitions) {
				printf("|tag=%ld size=%ld addr=%ld|", p.tag, p.size, p.addr);			
			}printf("\n");
			for (auto a = sim.free_blocks.begin(); a != sim.free_blocks.end(); a++) {
				printf("|tag:%ld size=%ld addr:%ld|", (*a)->tag, (*a)->size, (*a)->addr);
			}
			printf("\n");
			printf("\n %ld, %ld, %ld \n",sim.partitions.size(), sim.free_blocks.size(), sim.tagged_blocks.size());
    	}
  	}
		for (auto p : sim.partitions) {
			printf("|tag=%ld size=%ld addr=%ld|", p.tag, p.size, p.addr);			
		}printf("\n");
  	sim.getStats(result);
	
	//printf("\n\nLast memory: %ld\n\n", (*std::prev(sim.free_blocks.end()))->size);
}
