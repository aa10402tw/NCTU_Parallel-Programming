// OpenMP header 
#include <omp.h> 
#include <stdio.h> 
#include <stdlib.h> 
  
int main(int argc, char* argv[]) 
{ 
    int mat[100][100];
    for(int i=0; i<100; i++) 
        for(int j=0; j<100; j++)
            mat[i][j] = i+j;

    printf("Sum %d", compute_mat_sum(mat));
} 

int compute_mat_sum(int[100][100] mat){
    int sum = 0;
    for(int i=0; i<100; i++) 
        for(int j=0; j<100; j++)
            sum += mat[i][j]
    return sum;
}