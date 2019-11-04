// OpenMP header 
#include <omp.h> 
#include <stdio.h> 
#include <stdlib.h> 
  
int compute_mat_sum(int vec[]);

int main(int argc, char* argv[]) 
{ 
    int vec[10000];
    for(int i=0; i<10000; i++)
        vec[i] = i;
    printf("Sum %d", compute_mat_sum(vec));
} 

int compute_mat_sum(int vec[]){
    int sum = 0;
    for(int i=0; i<10000; i++)
        sum += vec[i];
    return sum;
}