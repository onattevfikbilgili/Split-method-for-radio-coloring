//sp1.cpp

#include "sp1.h"

int bigger(int x, int y){
	if(x > y){return x;}
	else{return y;}
}

Coloring sp1(unsigned int k, Graph & G, Graph & kdist){
	int num = G.num_nodes();
	std::vector<int> init(num,k * num);//we are using -1 for uncolored vertices
	Coloring out(init);
	for(unsigned int i = 0; i < num; i++){
		//std::cout << "Testing the coloring with vertex number " << i << " with the color 0\n";
		Coloring cur(init);
		std::vector<bool> S(num,false);//the set S from the algorithm
		cur.set(i,0);
		S[i] = true;
		int num_colored = 1;
		while(num_colored < num){
			int temp_min = k * num;
			int indx_min = num;
			for(int j = 0; j < num; j++){
				if(not S[j]){//if vertex j is not in S
					int temp = 0;
					for(auto neighbor: kdist.get_node(j).adjacent_nodes()){
						Graph::NodeId nbr_index = neighbor.id();
						int nbr_dist = (int)neighbor.edge_weight();
						if(S[nbr_index]){
							int candidate = cur.get(nbr_index) + bigger((k + 1) - nbr_dist,0);
							if(candidate > temp){
								temp = candidate;
							}
						}
					}
					if(temp < temp_min){
						indx_min = j;
						temp_min = temp;
					}
				}
			}
			cur.set(indx_min,temp_min);
			//std::cout << "Vertex number " << indx_min << " has been colored with " << temp_min << "\n";
			num_colored++;
			S[indx_min] = true;
		}
		//std::cout << "largest color used: " << cur.max() << "\n";
		if(cur.max() < out.max()){
			out = cur;
		}
		//std::cout << "\n";
	}
	return out;
}
