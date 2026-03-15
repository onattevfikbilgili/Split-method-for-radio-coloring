//coloring.cpp

#include "coloring.h"

//getters
int Coloring::get(unsigned int x){return colors[x];}
int Coloring::size(void){return size_;}
int Coloring::max(void){
	int max = colors[0];
	for(int i = 1; i < size_; i++){
		if(colors[i] > max){
			max = colors[i];
		}
	}
	return max;
}

//setters
void Coloring::set(unsigned int x, int y){
	colors[x] = y;
}

//constructor
Coloring::Coloring(std::vector<int> input){
	colors = input;
	size_ = input.size();
}

//print()
void Coloring::print(){
	for(int i = 0; i < size_; i++){
		std::cout << "Vertex with the index " << i << " has the color " << colors[i] << std::endl;
	}
}

int ham_dist(Coloring x, Coloring y){
        int n = x.size();
        int sum = 0;
        
        if(y.size() == n){
               for(int i = 0; i < n; i++){
                 sum += abs(x.get(i) - y.get(i));
               } 
        }
        return sum;
}
