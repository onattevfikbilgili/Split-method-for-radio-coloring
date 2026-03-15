//coloring.h
//Implementation of vertex colorings

#pragma once
#include <vector>
#include <iostream>

class Coloring{
	public:
		//getters
		int get(unsigned int x);
		int size(void);
		int max(void);

		//setters
		void set(unsigned int x, int y);

		//constructor
		Coloring(std::vector<int> input);

		//print
		void print();
	private:
		unsigned int size_;
		int max_;
		int max_index;
		std::vector<int> colors;
};

int ham_dist(Coloring x, Coloring y);
