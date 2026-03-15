//val_col_check.h - checks wether a given coloring is a valid graph radio k coloring

#include "graph.h"
#include "kgraph.h"
#include "coloring.h"
#include <iostream>

#pragma once

bool valid_coloring_check(Graph & g, unsigned int k, Coloring & c);

int count_violations_g_prime(Graph & g_prime, Coloring & c);
