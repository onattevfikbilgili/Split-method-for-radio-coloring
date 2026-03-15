//kgraph.cpp
//calculates G^k for a given undirected connected graph G

#include "kgraph.h"

//generates the graph that will be used for the heuristic
Graph kgraph(unsigned int k, Graph & G){
	//first step - calculating distances between nodes
	int num_vert = G.num_nodes();
	std::vector<int> allmax(num_vert,num_vert);
	std::vector<std::vector<int>> distances(num_vert,allmax);//all distances are set to maximamum at initiation
	for(int i = 0; i < num_vert; i++){//every vertex is at distances 0 to itself
		distances[i][i] = 0;
	}
	std::vector<bool> all_false(num_vert,false);
	for(int i = 0; i < num_vert; i++){//rub BFS for each vertex
		std::queue<Graph::NodeId> q;
		std::vector<bool> visited = all_false;//visited vertices are stored here
		visited[i] = true;//first vertex visited
		q.push(i);
		while(!q.empty()){
			auto cur_nodeid = q.front();
			q.pop();
			for (auto neighbor: G.get_node(cur_nodeid).adjacent_nodes()){
				if (not visited[neighbor.id()]) {
					visited[neighbor.id()] = true;
					distances[i][neighbor.id()] = distances[i][cur_nodeid] + 1;
					q.push(neighbor.id());
				}
			}
		}
	}
	//second step, create the k-th power graph
	Graph out(num_vert,Graph::undirected);
	for(int i = 0; i < num_vert; i++){
		for(int j = 0; j < num_vert; j++){
			if(distances[i][j] <= k and j > i){
				out.add_edge(i,j,(double)distances[i][j]);
			}
		}
	}
	return out;
}

//Generates the graph that will be used for the clustering
Graph kgraph2(Graph & G){
        Graph OG = kgraph(2,G);
        int n = G.num_nodes();
        Graph out(n,Graph::undirected);
        for(int i = 0; i < n; i++){
                for(auto neighbor: OG.get_node(i).adjacent_nodes()){
                        int u = neighbor.id();
                        if(u < i){
                                double c = neighbor.edge_weight();
                                if(c == 2){out.add_edge(i,u,1);}
                                else{out.add_edge(i,u,2);}
                        }
                        
                }
        }
        return out;
}
