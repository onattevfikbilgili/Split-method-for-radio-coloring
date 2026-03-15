//assignment.cpp

#include "assignment.h"

int smaller(int x, int y){
	if(x < y){return x;}
	else{return y;}
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Original ILP based Formulation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Coloring assignment_ilp(unsigned int k, Graph & G){
	int n = G.num_nodes();
	Graph kdist = kgraph(k,G);
	Coloring initial = sp1(k,G,kdist);//initial coloring using heuristic from Saha and Panigrahi
	int upper_bound = initial.max();//an upper bound on the radio-k chromatic number
	IloEnv myenv;
	IloModel mymodel(myenv);
	IloNumVar target(myenv, 0, upper_bound, ILOINT);//variable in the ILP representing the chromatic number
	IloNumVarArray variables(myenv, n * (upper_bound + 1), 0, 1, ILOINT);//variables x_{v,i} representing whether the vertex v has color i
										//variables[v * (upper_bound + 1) + i] represents x_{v,i}
	//Programm every vertex to have exactly one color
	for(int i = 0; i < n; i++){
		IloExpr onecolor(myenv);
		for(int j = 0; j <= upper_bound; j++){
			onecolor += variables[i * (upper_bound + 1) + j];
		}
		mymodel.add(IloRange(myenv, 1, onecolor, 1));
		onecolor.end();
	}
	//Programm the distance conditions
	for(int i = 0; i < n; i++){
		for(auto nbr: kdist.get_node(i).adjacent_nodes()){
			int nbr_index = nbr.id();
			int nbr_dist = nbr.edge_weight();
			if(nbr_index > i){//to ensure every edge {v,w} gets considered only once
				for(int color1 = 0; color1 <= upper_bound; color1++){
					for(int color2 = bigger(0, color1 - (k - nbr_dist)); color2 <= smaller(upper_bound, color1 + (k - nbr_dist)); color2++){
						mymodel.add(IloRange(myenv,0,variables[(i * (upper_bound + 1)) + color1] + variables[(nbr_index * (upper_bound + 1)) + color2],1));
					}
				}
			}
		}
	}
	//Programm the constraints on the variable representing the radio-k chromatic number
	for(int i = 0; i < n; i++){
		for(int j = 0; j <= upper_bound; j++){
			mymodel.add(IloRange(myenv,-IloInfinity, j * variables[i * (upper_bound + 1) + j] - target,0));
		}
	}
	//Programm the objective function
	mymodel.add(IloMinimize(myenv, target));

	//Solving the ILP model
	IloCplex mycplex(myenv);
	mycplex.extract(mymodel);
	mycplex.setOut(myenv.getNullStream());
	IloBool feasible = mycplex.solve();
	if(feasible == IloFalse){throw std::runtime_error("ILP not feasible!");}
	

	//Compute the coloring corresponding to the solution of our ILP
	std::vector<int> myvec(n,1234567890);
	Coloring out(myvec);
	for(int i = 0; i < n; i++){
		for(int j = 0; j <= upper_bound; j++){
			if(round(mycplex.getValue(variables[i * (upper_bound + 1) + j])) == 1){
				out.set(i,j);
			}
		}
	}

	mycplex.clear();
	myenv.end();

	//return the result
	return out;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function to solve the coloring problem within each cluster using ILP
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void split_ilp( Graph & G, int ub, std::vector<int> & labels, std::vector<std::vector<unsigned int>> & C, Coloring prev,Coloring & current, int cluster_number){
	int n = G.num_nodes();
	int k = 2;
	std::vector<unsigned int> cluster = C[cluster_number];
	int m = cluster.size();
	std::vector<int> order(n,m);
	for(int i = 0; i < m; i++){
		int node = cluster[i];
		order[node] = i;
	}
	
	//variables
	IloEnv myenv;
	IloModel mymodel(myenv);
	IloNumVar lambda(myenv, 0, ub, ILOINT);//maximal color
	IloNumVarArray variables(myenv, m * (ub + 1), 0, 1, ILOINT);
	IloExpr target(myenv);
	target += lambda;
	
	//Programm every vertex to have exactly one color
	for(int i = 0; i < m; i++){
		IloExpr onecolor(myenv);
		for(int j = 0; j <= ub; j++){
			onecolor += variables[i * (ub + 1) + j];
		}
		mymodel.add(IloRange(myenv, 1, onecolor, 1));
		onecolor.end();
	}
	
	//Programm the distance conditions and penalties
	for(int i = 0; i < m; i++){
		int u = cluster[i];//our vertex is the i-th node in the cluster that is u
		for(auto nbr: G.get_node(u).adjacent_nodes()){//get the adjacent nodes of u in the 2 graph
			int v = nbr.id();
			int dist = nbr.edge_weight();
			if(v > u){//skip double counting of edges
				if(labels[v] == cluster_number){//nbr in the cluster --> add constraint
					int order_v = order[v];
					for(int color1 = 0; color1 <= ub; color1++){
						for(int color2 = bigger(0, color1 - (k - dist)); color2 <= smaller(ub, color1 + (k - dist)); color2++){
							mymodel.add(IloRange(myenv,0,variables[(i * (ub + 1)) + color1] + variables[(order_v * (ub + 1)) + color2],1));
						}
					}
				}
				else{//nbr not in the cluster --> add penalty
					int color_nbr = prev.get(v);
					if(dist == 2){
						target += (ub / 2) * variables[i * (ub + 1) + color_nbr];
					}
					else{
						for(int color2 = bigger(0, color_nbr - (2 - 1)); color2 <= smaller(ub, color_nbr + (2 - 1)); color2++){
							target += (ub / 2) * variables[i * (ub + 1) + color2];
						}
					}
					
				}
			}
		}
	}
	
	//Programm the constraints on the variable representing the radio-k chromatic number
	for(int i = 0; i < m; i++){
		for(int j = 0; j <= ub; j++){
			mymodel.add(IloRange(myenv,-IloInfinity, j * variables[i * (ub + 1) + j] - lambda,0));
		}
	}
	
	//Solving the ILP model
	mymodel.add(IloMinimize(myenv, target));
	IloCplex mycplex(myenv);
	mycplex.extract(mymodel);
	mycplex.setOut(myenv.getNullStream());
	IloBool feasible = mycplex.solve();
	if(feasible == IloFalse){throw std::runtime_error("ILP not feasible!");}
	
	//Compute the coloring corresponding to the solution of our ILP
	for(int i = 0; i < m; i++){
		for(int j = 0; j <= ub; j++){
			if(round(mycplex.getValue(variables[i * (ub + 1) + j])) == 1){
				current.set(order[i],j);
			}
		}
	}
	
	mycplex.clear();
	myenv.end();
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Sweep update process
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int sweep_update(Graph & G, Coloring & c, int ub){
	int val1 = ub * count_violations_g_prime(G,c);
	int max = c.max();
	int total = max + val1;
	int n = G.num_nodes();
	Coloring newc = c;
	int number = n / 10;
	srand(time(0));
	for(int i = 0; i < number; i++){
		int random_node = rand() % n;
		int newcolor = (c.get(random_node) + rand()) % max;
		newc.set(random_node,newcolor);
	}
	int tot2 = newc.max() + ub * count_violations_g_prime(G,newc);
	if(tot2 < total){c = newc; total = tot2;}
	return total;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Main function for the SPLIT Process
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Coloring assignment_split(Graph & G){
        int n = G.num_nodes();
	Graph kdist = kgraph(2,G);
	Graph k2 = kgraph2(G);
	Coloring initial = sp1(2,G,kdist);//initial coloring using heuristic from Saha and Panigrahi
	int upper_bound = initial.max();//an upper bound on the radio-2 chromatic number
	
	//timing
	//automatic termination on exceeding the given time limit, adjust the timeoutmeter variable to liking (it is in minutes)
	int timeoutmeter = 30;
	auto start = std::chrono::steady_clock::now();
	auto timeout = std::chrono::minutes(timeoutmeter);
	
	
	//Clustering
	GraphClusterer clusterer;
	std::vector<int> labels = clusterer.clusterGraph(k2, 4);
	std::vector<std::vector<unsigned int>> C ={{},{},{},{}};
	for(int i = 0; i < n; i++){
	        int l = labels[i];
	        C[l].push_back(i);
	}
	
	
	//initialize the SPLIT Framework
	int prev_score = upper_bound;
	Coloring current = initial;
	Coloring prev = current;
	Coloring best = current;
	
	unsigned int no_improvements = 0;
	int iteration = 0;
	while(std::chrono::steady_clock::now() - start < timeout and iteration < 20 and no_improvements < 5){//convergence criteria
		
		//Solve all cluster based coloring problems in paralel
	        std::thread t1([&]{split_ilp(kdist,upper_bound,labels,C,prev,current,0);
	        });
	        
	        std::thread t2([&]{split_ilp(kdist,upper_bound,labels,C,prev,current,1);
	        });
	        
	        std::thread t3([&]{split_ilp(kdist,upper_bound,labels,C,prev,current,2);
	        });
	        
	        std::thread t4([&]{split_ilp(kdist,upper_bound,labels,C,prev,current,3);
	        });
	        
	        t1.join();
	        t2.join();
	        t3.join();
	        t4.join();
	        //sweep update and test convergence criteria
	        int cur_val = sweep_update(k2,current,upper_bound);
	        double asdf = abs(cur_val - prev_score);
	        double rte = prev_score;
	        rte *= 0.05;
	        if(asdf < rte){no_improvements++;}
	        else{no_improvements = 0;}
	        if(valid_coloring_check(G,2,current)){
	        	int nub = current.max();
	        	if(nub < upper_bound){
	        		upper_bound = nub;
	        		best = current;
	        	}
	        }
	        
	        prev = current;
	        prev_score = cur_val;
	        iteration++;
	}
	//std::cout << "Number of iterations is "<< iteration << std::endl;
	
	return best;
}
