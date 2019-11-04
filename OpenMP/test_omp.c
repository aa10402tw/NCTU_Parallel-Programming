// OpenMP header 
#include <omp.h> 
#include <stdio.h> 
#include <stdlib.h> 
  
int compute_mat_sum(int vec[]);

int main(int argc, char* argv[]) 
{ 
    int vec[1000000];
    for(int i=0; i<1000000; i++)
        vec[i] = i;
    printf("Sum %d", compute_mat_sum(vec));
} 

int compute_mat_sum(int vec[]){
    int sum = 0, i;
    omp_set_num_threads(1);
    #pragma omp parallel for reduction (+:sum)
    for(int i=0; i<1000000; i++)
        sum += vec[i];
    return sum;
}