// graph_cluster.cpp
#include "graph_cluster.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <map>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Helper function to convert Python error to string
std::string getPythonError() {
    PyObject *type, *value, *traceback;
    PyErr_Fetch(&type, &value, &traceback);
    
    std::string errorMsg = "Unknown Python error";
    if (value != nullptr) {
        PyObject* str = PyObject_Str(value);
        if (str != nullptr) {
            errorMsg = PyUnicode_AsUTF8(str);
            Py_DECREF(str);
        }
    }
    
    PyErr_Restore(type, value, traceback);
    return errorMsg;
}

GraphClusterer::GraphClusterer() : pythonModule(nullptr), pythonFunction(nullptr) {
    initializePython();
}

GraphClusterer::~GraphClusterer() {
    // Clean up Python objects
    if (pythonFunction != nullptr) {
        Py_DECREF(reinterpret_cast<PyObject*>(pythonFunction));
    }
    if (pythonModule != nullptr) {
        Py_DECREF(reinterpret_cast<PyObject*>(pythonModule));
    }
    
    // Finalize Python interpreter (only if no other Python code is running)
    if (Py_IsInitialized()) {
        Py_Finalize();
    }
}

void GraphClusterer::initializePython() {
    // Initialize Python interpreter
    if (!Py_IsInitialized()) {
        Py_Initialize();
        if (!Py_IsInitialized()) {
            throw std::runtime_error("Failed to initialize Python interpreter");
        }
        
        // Add current directory to Python path
        PyRun_SimpleString("import sys\nsys.path.append('.')");
    }
    
    // Import the Python module
    PyObject* pName = PyUnicode_DecodeFSDefault("spectral_cluster_helper");
    if (!pName) {
        throw std::runtime_error("Failed to create module name: " + getPythonError());
    }
    
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    
    if (!pModule) {
        PyErr_Print();
        throw std::runtime_error("Failed to import Python module 'spectral_cluster_helper'."
                                 " Make sure the file exists in current directory.");
    }
    
    pythonModule = pModule;
    
    // Get reference to the clustering function
    PyObject* pFunc = PyObject_GetAttrString(pModule, "cluster_graph");
    if (!pFunc || !PyCallable_Check(pFunc)) {
        if (pFunc) Py_DECREF(pFunc);
        Py_DECREF(pModule);
        throw std::runtime_error("Failed to get 'cluster_graph' function from Python module");
    }
    
    pythonFunction = pFunc;
}

std::vector<std::vector<double>> GraphClusterer::buildAdjacencyMatrix(Graph& graph) {
    int n = graph.num_nodes();
    std::vector<std::vector<double>> adj_matrix(n, std::vector<double>(n, 0.0));
    
    // Fill adjacency matrix (assuming undirected graph)
    for (int i = 0; i < n; ++i) {
        const auto& node = graph.get_node(i);
        for (const auto& neighbor : node.adjacent_nodes()) {
            int j = neighbor.id();
            double weight = neighbor.edge_weight();
            adj_matrix[i][j] = weight;
            adj_matrix[j][i] = weight;  // Symmetric for undirected
        }
    }
    
    return adj_matrix;
}

void* GraphClusterer::matrixToPython(const std::vector<std::vector<double>>& matrix) {
    PyObject* pMatrix = PyList_New(matrix.size());
    if (!pMatrix) {
        throw std::runtime_error("Failed to create Python list: " + getPythonError());
    }
    
    for (size_t i = 0; i < matrix.size(); ++i) {
        PyObject* pRow = PyList_New(matrix[i].size());
        if (!pRow) {
            Py_DECREF(pMatrix);
            throw std::runtime_error("Failed to create Python row: " + getPythonError());
        }
        
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            PyObject* pValue = PyFloat_FromDouble(matrix[i][j]);
            if (!pValue) {
                Py_DECREF(pRow);
                Py_DECREF(pMatrix);
                throw std::runtime_error("Failed to create Python float: " + getPythonError());
            }
            PyList_SetItem(pRow, j, pValue);  // Steals reference to pValue
        }
        PyList_SetItem(pMatrix, i, pRow);  // Steals reference to pRow
    }
    
    return pMatrix;
}

std::vector<int> GraphClusterer::pythonToLabels(void* pythonLabels) {
    PyObject* pList = reinterpret_cast<PyObject*>(pythonLabels);
    std::vector<int> labels;
    
    Py_ssize_t size = PyList_Size(pList);
    labels.reserve(size);
    
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* pItem = PyList_GetItem(pList, i);  // Borrowed reference
        long label = PyLong_AsLong(pItem);
        if (PyErr_Occurred()) {
            throw std::runtime_error("Failed to convert Python label to integer: " + getPythonError());
        }
        labels.push_back(static_cast<int>(label));
    }
    
    return labels;
}

std::vector<int> GraphClusterer::clusterGraph(Graph& graph, int n_clusters) {
    if (!pythonFunction) {
        throw std::runtime_error("Python function not initialized");
    }
    
    // Build adjacency matrix
    std::vector<std::vector<double>> adj_matrix = buildAdjacencyMatrix(graph);
    
    // Convert to Python object
    PyObject* pMatrix = reinterpret_cast<PyObject*>(matrixToPython(adj_matrix));
    
    // Prepare arguments: (adjacency_matrix, n_clusters)
    PyObject* pArgs = PyTuple_New(2);
    if (!pArgs) {
        Py_DECREF(pMatrix);
        throw std::runtime_error("Failed to create Python arguments tuple: " + getPythonError());
    }
    
    // Set first argument (adjacency matrix)
    PyTuple_SetItem(pArgs, 0, pMatrix);  // Steals reference
    
    // Set second argument (n_clusters)
    PyObject* pNClusters = PyLong_FromLong(n_clusters);
    if (!pNClusters) {
        Py_DECREF(pArgs);
        throw std::runtime_error("Failed to create n_clusters argument: " + getPythonError());
    }
    PyTuple_SetItem(pArgs, 1, pNClusters);  // Steals reference
    
    // Call Python function
    PyObject* pResult = PyObject_CallObject(reinterpret_cast<PyObject*>(pythonFunction), pArgs);
    Py_DECREF(pArgs);
    
    if (!pResult) {
        PyErr_Print();
        throw std::runtime_error("Python function call failed: " + getPythonError());
    }
    
    // Convert result to C++ vector
    std::vector<int> labels = pythonToLabels(pResult);
    Py_DECREF(pResult);
    
    return labels;
}


void printClustering(const std::vector<int>& labels) {
    std::cout << "Clustering results (" << labels.size() << " vertices):\n";
    std::cout << "Vertex -> Cluster\n";
    std::cout << "------------------\n";
    
    for (size_t i = 0; i < labels.size(); ++i) {
        std::cout << "  " << i << "   ->   " << labels[i] << "\n";
    }
    
    // Print cluster sizes
    std::cout << "\nCluster sizes:\n";
    std::cout << "--------------\n";
    
    std::map<int, int> clusterCounts;
    for (size_t i = 0; i < labels.size(); ++i) {
        int label = labels[i];
        clusterCounts[label]++;
    }
    
    for (std::map<int, int>::const_iterator it = clusterCounts.begin(); 
         it != clusterCounts.end(); ++it) {
        std::cout << "Cluster " << it->first << ": " << it->second << " vertices\n";
    }
}
