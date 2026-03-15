// graph_cluster.h
#ifndef GRAPH_CLUSTER_H
#define GRAPH_CLUSTER_H

#include <vector>
#include <string>
#include "graph.h"

//Partition the vertices of a graph into 4 clusters using python scikit libraries
class GraphClusterer {
public:
    //Constructor
    GraphClusterer();
    
    //Destructor
    ~GraphClusterer();
    
    //Partitions the graph into 4 clusters
    std::vector<int> clusterGraph(Graph& graph, int n_clusters = 4);
    
    
    // Disable copying (Python interpreter state is non-copyable)
    GraphClusterer(const GraphClusterer&) = delete;
    GraphClusterer& operator=(const GraphClusterer&) = delete;
    
private:
    // Python objects (opaque pointers to avoid including Python.h in header)
    void* pythonModule;   // PyObject* for the module
    void* pythonFunction; // PyObject* for the function
    
    // Initialize Python interpreter and load sklearn module
    void initializePython();
    
    //BUild adjacency matrix
    std::vector<std::vector<double>> buildAdjacencyMatrix(Graph& graph);
    
    //Convert c++ adjancy matrix to python
    void* matrixToPython(const std::vector<std::vector<double>>& matrix);
    
    //convert python data to C++ labels
    std::vector<int> pythonToLabels(void* pythonLabels);
};

//print
void printClustering(const std::vector<int>& labels);



#endif // GRAPH_CLUSTER_H
