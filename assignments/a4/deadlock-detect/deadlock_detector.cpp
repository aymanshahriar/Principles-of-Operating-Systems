// this is the ONLY file you should edit and submit to D2L

#include "common.h"
#include "deadlock_detector.h"

/// this is the function you need to implement
/// parameter edges[] contains a list of request and assignment edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
/// Parameters edge_index and cycle[] are used to return
/// results back to the caller.
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge.
///
/// As soon as you detect a deadlock, you need to set edge_index to the index of the edge
/// in edges[] that caused the deadlock. For example, edges[7] caused the deadlock,
/// then set edge_index=7. You must also populate cycle[] with the names of
/// processes that are in a deadlock. You can then return from the function without
/// processing any other edges.
///
/// To indicate deadlock was detected after processing all edges, you must
/// set edge_index = -1 and clear cycle[] e.g. by calling cycle.clear()
///


class Graph {
	public:
		// Incoming nodes of each node
		std::vector<std::vector<int>> incoming_nodes; 
	
		// Outgoing degree of each node
		std::vector<int> outgoing_degree;
};





// currently, we can successfully add all the edges to a graph.
// TODO: at each iteration, after adding the new edge, check if there is a cycle


void detect_deadlock(const std::vector<std::string> & edges, int & edge_index, std::vector<std::string> & cycle) {
	// Helper variables
	std::vector<std::string> words;
	std::vector<int> out;
	std::vector<int> zeroes;
	int n;

	// Set edge_index to -1, and cycle to be empty vector
	edge_index = -1;
	cycle = std::vector<std::string>();

	// Initialize empty graph
	Graph graph;

	Word2Int w2i;
	int P, R;
	std::unordered_map<int, std::string> stringNames;
	
	for (int i = 0; i < (int) edges.size(); i++) {
		
		// For edge at edges[i], obtain the process, resource, and whether it's a request or an assignment
		words = split(edges[i]);

		// Add "$" to the start of the process name, so all resource names will not start with "$"
		words[0] = (std::string)"$" + words[0];

		// Convert the string names of the process and resource into integer names
		// Store string names inside stringNames[] with the integer name as key
		// We will need stringNames[] when we are converting the integer name back to string name when filling in cycle[]
		P = w2i.get(words[0]);
		R = w2i.get(words[2]);
		stringNames[P] = words[0];
		stringNames[R] = words[2];

	
		// If P or R is a new node, then make room for them in the two vectors
		if (P >= (int)graph.outgoing_degree.size()) {
			graph.outgoing_degree.push_back(0);
			graph.incoming_nodes.push_back(std::vector<int>());
		}
		if (R >= (int)graph.outgoing_degree.size()) {
            graph.outgoing_degree.push_back(0);
            graph.incoming_nodes.push_back(std::vector<int>());
        }

		// Check if it's a request edge, then add edge to graph
		if (words[1][0] == '-') {
			
			// update P's value in outgoing_degree:
			graph.outgoing_degree[P]++;
			
			// update R's list of incoming_nodes:
			graph.incoming_nodes[R].push_back(P);
		}

		// Otherwise, it must be an assignment edge
		else { 
		
			// update R's value in outgoing_degree:
            graph.outgoing_degree[R]++;

            // update P's list of incoming_nodes:
            graph.incoming_nodes[P].push_back(R);
		}


        // ****Up till this point, I have checked everything*****

        // run topological sort algorithm to check for cycle
		out = graph.outgoing_degree;
		zeroes = std::vector<int>();		
		
		// Find all nodes in graph with outgoing degree of 0
		for (int i = 0; i < (int) graph.outgoing_degree.size(); i++) {
			if (graph.outgoing_degree[i] == 0) {
				zeroes.push_back(i);
			}			
		}
		
		while (zeroes.size() > 0) {
			n = zeroes.back();
			zeroes.pop_back();

			for (int n2 : graph.incoming_nodes[n]) {
    	        out[n2]--;
				if (out[n2] == 0) zeroes.push_back(n2);
	        }
		}
		
		
		// In cycle[], collect all nodes in graph with out[n]>0 and is_process(n).
		for (int i = 0; i < (int) out.size(); i++) {
			if ((out[i] > 0) && (stringNames[i][0] == '$')) {
				// make sure you remove the "$"
				cycle.push_back(stringNames[i].substr(1));
			}
		}
				
		// If cycle contains some nodes, then set index to i of the big for loop. Then break
		if (cycle.size() > 0) {
			edge_index = i;
			break;
		}
		


	}  // end of big for loop
	


/*	
	printf("\ncontents of outgoing_degree:\n");
	for (int i = 0; i < (int)graph.outgoing_degree.size(); i++) {
		printf("%s: %d\n", stringNames[i].c_str(), graph.outgoing_degree[i]);
	}
	
	printf("\ncontents of incoming_nodes:\n");
	for (int i = 0; i < (int)graph.incoming_nodes.size(); i++) {
        printf("%s: [", stringNames[i].c_str());
		for (int x : graph.incoming_nodes[i]) {
			printf("%s, ", stringNames[x].c_str());
		}
		printf("]\n");
    }
*/	
	


/*	
    // let's try to guess the results :)
    cycle = split("12 7 7");
    edge_index = 6;
*/	

}










/*
 	// See if graph is properly initialized:
	printf("%ld %ld\n", graph.incoming_nodes.size(), graph.outgoing_degree.size());
	
	// Check if loop iterates over all element indexes of edges
	printf("%ld\n", i);

	// Check if each edge is correctly split
	printf("Process: '%s', req/assign: '%s', resource: '%s'\n", stringNames[P].c_str(), words[1].c_str(), stringNames[R].c_str());

	// Check if we can correctly determine if edge is assignment edge of request edge
	        
    if (words[1][0] == '-') {
     	printf("request edge\n");
   	}

    else {
   		printf("assignment edge\n");
    }

 
 */
