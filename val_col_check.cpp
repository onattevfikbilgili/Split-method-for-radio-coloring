//val_col_check.cpp

#include "val_col_check.h"

bool valid_coloring_check(Graph & g, unsigned int k, Coloring & c){
	Graph kdist = kgraph(k,g);
	int n = kdist.num_nodes();
	for(int i = 0; i < n; i++){
		for(auto nbr : kdist.get_node(i).adjacent_nodes()){
			int nbr_id = nbr.id();
			int dist = (int)nbr.edge_weight();
			if(i > nbr_id){//to avoid checking for the same pair of vertices twice
				int color1 = c.get(i);
				int color2 = c.get(nbr_id);
				if(abs(color1 - color2) <= (k - dist)){
					//std::cout << "vertices: "  << i << "-" << nbr_id << " colors: " << color1 << " and " << color2 << " distance: " << dist << " and k is: "<< k <<"\n";
					return false;
				}
			}
		}
	}
	return true;
}

// In val_col_check.cpp
int count_violations_g_prime(Graph & g_prime, Coloring & c) {
    int n = g_prime.num_nodes();
    int violation_count = 0;
    
    for (int i = 0; i < n; i++) {
        for (auto neighbor : g_prime.get_node(i).adjacent_nodes()) {
            int j = neighbor.id();
            if (i > j) continue; // Count each edge only once
            
            double weight = neighbor.edge_weight();
            int color_i = c.get(i);
            int color_j = c.get(j);
            
            if (weight == 2.0) {
                if (std::abs(color_i - color_j) < 2) violation_count++;
            } else if (weight == 1.0) {
                if (std::abs(color_i - color_j) < 1) violation_count++;
            }
        }
    }
    
    return violation_count;
}
