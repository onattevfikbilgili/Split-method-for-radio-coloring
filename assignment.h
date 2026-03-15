//assignment.h
//Implementation of SPLIT framework for the radio coloring problem
#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <thread>
#include <ctime>
#include <vector>
#include "graph.h"
#include "kgraph.h"
#include "coloring.h"
#include "sp1.h"
#include "graph_cluster.h"
#include "val_col_check.h"

#pragma once

int smaller(int x, int y);

Coloring assignment_ilp(unsigned int k, Graph & G);

void split_ilp( Graph & G, int ub, std::vector<int> & labels, std::vector<std::vector<unsigned int>> & C, Coloring prev, Coloring & current, int cluster_number);

int sweep_update(Graph & G, Coloring & c, int ub);

Coloring assignment_split(Graph & G);
