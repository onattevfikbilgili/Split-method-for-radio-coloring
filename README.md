#SPLIT Method for radio-2 vertex coloring

This is the implementation of a paralelized algorithm for radio-2 vertex colorings of graphs. The file Mathematical_explanation.pdf contains all the details on how it works.

All the test instances used can be found in the directory named instances.

The implementation of graph structures are taken from https://www.or.uni-bonn.de/~hougardy/alma/alma_eng.html based on the book "Algorithmic Mathematics" by Jens Vygen and Stefan Hougardy.

Hougardy, S. and Vygen, J.,Algorithmic Mathematics, (2016) 80-83

Scikit-learn libraries were used for spectral clustering.

This package makes use of CPLEX to solve intiger-linear programs. To compile those add the necessary CPLEX files to include and lib directories respectively (After creating those directories).

default usage:

./split-assign instance_name
