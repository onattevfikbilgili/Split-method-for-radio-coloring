#!/bin/sh
#Version 2211 of cplex studio was used for this project
#all necessary files for cplex are included within the directories include and lib
#computes a graph radio-k coloring of the input graph
#to run the programm type $./assign FILENAME k
#in your shell where FILENAME is the name of the file where the graph is stored, and k is a natural number greater than 0
g++ -I./include -o split-assign main.cpp graph.cpp kgraph.cpp coloring.cpp sp1.cpp assignment.cpp val_col_check.cpp graph_cluster.cpp \
    -I/usr/include/python3.12 \
    -L/usr/lib/python3.12/config-3.12-x86_64-linux-gnu \
    -lpython3.12 -L./lib -lilocplex -lconcert -lcplex -lm -lpthread
