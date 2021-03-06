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
		std::unordered_map<std::string, std::vector<std::string>> incoming_nodes; 
	
		// Outgoing degree of each node
		std::unordered_map<std::string, int> outgoing_degree;
};





// currently, we can successfully add all the edges to a graph.
// TODO: at each iteration, after adding the new edge, check if there is a cycle


void detect_deadlock(const std::vector<std::string> & edges, int & edge_index, std::vector<std::string> & cycle) {
	// Helper variables
	std::vector<std::string> words;
	std::unordered_map<std::string, int> out;
	std::vector<std::string> zeroes;
	std::string n;

	// Set edge_index to -1, and cycle to be empty vector
	edge_index = -1;
	cycle = std::vector<std::string>();

	// Initialize empty graph
	Graph graph;
	
	for (int i = 0; i < (int) edges.size(); i++) {
		
		// For edge at edges[i], obtain the process, resource, and whether it's a request or an assignment
		words = split(edges[i]);

		// Add "$" to the start of the process name, so all resource names will not start with "$"
		words[0] = (std::string)"$" + words[0];
	
		// Check if it's a request edge, then add edge to graph
		if (words[1][0] == '-') {
			graph.outgoing_degree[words[0]]++;
			graph.incoming_nodes[words[2]].push_back(words[0]);

			// make sure process node is included in incoming_nodes
			if (graph.incoming_nodes.find(words[0]) == graph.incoming_nodes.end()) {
				graph.incoming_nodes.insert(std::pair<std::string, std::vector<std::string>>(words[0], std::vector<std::string>()));
			}

			// make sure assignment node is included in outgoing_degree
			if (graph.outgoing_degree.find(words[2]) == graph.outgoing_degree.end()) {
				graph.outgoing_degree[words[2]] = 0;
			}			
			
		}

		// Otherwise, it must be an assignment edge
		else {
			graph.outgoing_degree[words[2]]++;
            graph.incoming_nodes[words[0]].push_back(words[2]);
			
            // make sure assignment node is included in incoming_nodes
            if (graph.incoming_nodes.find(words[2]) == graph.incoming_nodes.end()) {
                graph.incoming_nodes.insert(std::pair<std::string, std::vector<std::string>>(words[2], std::vector<std::string>()));
            }

            // make sure process node is included in outgoing_degree
            if (graph.outgoing_degree.find(words[0]) == graph.outgoing_degree.end()) {
                graph.outgoing_degree[words[0]] = 0;
            }

		}
		
        // ****Up till this point, I have checked everything*****

        // run topological sort algorithm to check for cycle
		out = graph.outgoing_degree;
		zeroes = std::vector<std::string>();		
		
		// Find all nodes in graph with outgoing degree of 0
		for (std::pair<std::string, int> pair : graph.outgoing_degree) {
			if (pair.second == 0) {
				zeroes.push_back(pair.first);
			}
		}
		
		while (zeroes.size() > 0) {
			n = zeroes.back();
			zeroes.pop_back();

			for (std::string n2 : graph.incoming_nodes[n]) {
    	        out[n2]--;
				if (out[n2] == 0) zeroes.push_back(n2);
	        }
		}
		
		
		// In cycle[], collect all nodes in graph with out[n]>0 and is_process(n).
		for (std::pair<std::string, int> pair : out) {
			if ((pair.second > 0) && (pair.first[0] == '$')) {
				// make sure you remove the "$"
				cycle.push_back(pair.first.substr(1));
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
	for (auto pair : graph.outgoing_degree) {
		printf("%s: %d\n", pair.first.c_str(), pair.second);
	}

	printf("\ncontents of incoming_nodes:\n");
	for (auto pair : graph.incoming_nodes) {
        printf("%s: [", pair.first.c_str());
		for (std::string str : pair.second) {
			printf("%s, ", str.c_str());
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
	printf("Process: '%s', req/assign: '%s', resource: '%s'\n", words[0].c_str(), words[1].c_str(), words[2].c_str());

	// Check if we can correctly determine if edge is assignment edge of request edge
	        
    if (words[1][0] == '-') {
     	printf("request edge\n");
   	}

    else {
   		printf("assignment edge\n");
    }

 
 */
