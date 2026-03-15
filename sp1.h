//sp1.h - Implementation of the heuristic from Saha and Panigrahi for the graph radio-k coloring problem

#pragma once
#include <vector>
#include <iostream>
#include "graph.h"
#include "kgraph.h"
#include "coloring.h"

#pragma once

int bigger(int x, int y);

Coloring sp1(unsigned int k, Graph & G, Graph & kdist);
