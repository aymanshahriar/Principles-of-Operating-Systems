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
};


// An iterator(pointer) to a list of partitions
typedef std::list<Partition>::iterator PartitionRef;


// used to order set of empty partitions
struct scmp {
	bool operator()( const PartitionRef & c1, const PartitionRef & c2) const {
	if( c1->size == c2->size)
		return c1->addr < c2->addr;
	else
		return c1-> size < c2-> size;
	}
};



// feel free to use this class as a starting point
struct Simulator {
	int64_t page_size;
	int64_t max_free_partition_size;
	int64_t n_pages_requested;

	// all partitions (to get std::prev() and std::next() nodes)
	std::list<Partition> partitions;

	// pointers to free partitions, sorted by size/address
	std::set<PartitionRef,scmp> free_blocks;

	// Map that contains all occupied partitions (for quick access to all tagged partitions)
	std::unordered_map<long, std::vector<PartitionRef>> tagged_blocks;

	// constructor
  	Simulator(int64_t page_size_) { page_size = page_size_; }

  	void allocate(int tag, int size) { printf("allocating tag=%d size=%d\n", tag, size); }

  	void deallocate(int tag) { printf("deallocating tag=%d\n", tag); }

  	void getStats(MemSimResult & result) {
    	// return the size of the maximum free partition (set to 0 if no free partitions exist)
    	result.max_free_partition_size = 123;
    	// return the total number of pages requested
    	result.n_pages_requested = 9;
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
  	for (const auto & req : requests) {
    	if (req.tag < 0) {
      		sim.deallocate(-req.tag);
    	} else {
      		sim.allocate(req.tag, req.size);
    	}
  	}
  	sim.getStats(result);
}
