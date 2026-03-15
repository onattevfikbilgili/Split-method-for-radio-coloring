//the main file for the radio-2 coloring algorithm using an assignment based ILP formulation and the SPLIT framework
#include <iostream>
#include <cstdlib>
#include <chrono>
#include "graph.h"
#include "kgraph.h"
#include "coloring.h"
#include "sp1.h"
#include "assignment.h"
#include "val_col_check.h"
#include "graph_cluster.h"

int main(int argc, char** argv){
	if(argc > 1){
		Graph g(argv[1],Graph::undirected);
		unsigned int k = 2;
		std::chrono::time_point<std::chrono::system_clock> start, end;
		start = std::chrono::system_clock::now();
		Coloring split = assignment_split(g);
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		if(valid_coloring_check(g,k,split)){
			std::cout << "Printing a radio-" << k << " coloring of the original graph:\n";
			split.print();
			std::cout << "Largest color used is " << split.max() << "\n";
			std::cout << "Took " << elapsed_seconds.count() << " seconds to compute the coloring.\n";
		}
	}
	return 0;
}
