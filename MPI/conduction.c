#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef W
#define W 20                                   // Width
#endif

#ifndef MASTER_RANK 
#define MASTER_RANK 0
#endif 

#define isDebug 0

void syncBoundaryRow(int* mat, int row_start, int row_end, int my_rank, int world_size) {
    // Send start row to previous node
    if(my_rank > 0) 
      MPI_Send( &mat[row_start*W], W, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD);

    // Send end row to next node
    if(my_rank+1 < world_size)
      MPI_Send( &mat[(row_end-1)*W], W, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD);

    // Get (start-1) row from previous node
    if(my_rank+1 < world_size)
      MPI_Recv( &mat[row_end*W], W, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    // Get (end+1) row from next node
    if(my_rank > 0)
      MPI_Recv( &mat[(row_start-1)*W], W, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

int findMatMin(int* mat, int row_start, int row_end) {
  int min = mat[row_start*W];
  for(int i=row_start; i<row_end; i++) {
    for(int j=0; j<W; j++) {
      if(mat[i*W+j] < min) 
        min = mat[i*W+j];
    }
  }
  return min;
}

int main(int argc, char **argv) {

  /* MPI World Init */
  MPI_Init(NULL, NULL);
  MPI_Barrier(MPI_COMM_WORLD);

  int world_size, my_rank, name_len;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(processor_name, &name_len);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Get_processor_name(processor_name, &name_len);

  MPI_Barrier(MPI_COMM_WORLD);
  if(isDebug){
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Process Rank:(%d/%d), from host [%s]\n", my_rank+1, world_size, processor_name);
    MPI_Barrier(MPI_COMM_WORLD);
  }

  /* Initialize arguments */
  int L, iteration, seed;
  float d;
  int *temp;
  int *next;
  if(my_rank == MASTER_RANK) {
    L = atoi(argv[1]);                        // Length
    iteration = atoi(argv[2]);                // Iteration
    seed = atoi(argv[3]);                     // Seed
    srand(seed);                     
    d = (float) random() / RAND_MAX * 0.2;    // Diffusivity
    for(int i=1; i<world_size; i++) {
      MPI_Send( &L,         1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send( &iteration, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send( &seed,      1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    temp = malloc(L*W*sizeof(int));          // Current temperature
    next = malloc(L*W*sizeof(int));          // Next time step
  }
  else{
    MPI_Recv( &L,         1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv( &iteration, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv( &seed,      1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    srand(seed);                     
    d = (float) random() / RAND_MAX * 0.2;    // Diffusivity
    temp = malloc(L*W*sizeof(int));           // Current temperature
    next = malloc(L*W*sizeof(int));           // Next time step
  }
  
  // Random Init Temperature
  for (int i = 0; i < L; i++) {
    for (int j = 0; j < W; j++) {
      temp[i*W+j] = random()>>3;
    }
  }

  /* Start Conduction */
  int count = 0, local_balance = 0, global_balance=0;
  int n_row = L / world_size;
  int row_start = n_row*my_rank;
  int row_end   = n_row*my_rank+n_row;
  if(isDebug) {
    MPI_Barrier(MPI_COMM_WORLD);
    printf("conduction from %d, process:(start:%d, end:%d)\n", my_rank, row_start, row_end);
    MPI_Barrier(MPI_COMM_WORLD);
  }
  while (iteration--) {     // Compute with up, left, right, down points
    local_balance = 1;
    count++;
    for (int i = row_start; i < row_end; i++) {
      for (int j = 0; j < W; j++) {
        /*
        t_new = t + Diffusivity * (d1 + d2 + d3 + d4)
        => t_new = t + Diffusivity * ( (t1-t) + (t2-t) + (t3-t) + (t4-t) )
        => t_new = Diffusivity * [ (t/Diffusivity) + (t1 + t2 + t3 + t4) - 4*(t) ]
        => t_new = [(t/Diffusivity) - 4*(t) + (t1 + t2 + t3 + t4)] * Diffusivity
        */

        // t_new = (t/Diffusivity)
        float t = temp[i*W+j] / d; 
        
        // t_new = t_new - 4*(t)
        t += temp[i*W+j] * -4; 

        // t_new = t_new + (t1 + t2 + t3 + t4)
        t += temp[(i - 1 <  0 ? 0 : i - 1) * W + j];
        t += temp[(i + 1 >= L ? i : i + 1) * W + j];
        t += temp[i * W + (j - 1 <  0 ? 0 : j - 1)];
        t += temp[i * W + (j + 1 >= W ? j : j + 1)];

        // t_new = t_new * Diffusivity 
        t *= d;
        next[i*W+j] = t ;

        // If tempature change, it's not balance yet
        if (next[i*W+j] != temp[i*W+j]) {
          local_balance = 0;
        }
      }
    }
    
    // Synchronize boundary row
    MPI_Barrier(MPI_COMM_WORLD);
    syncBoundaryRow(next, row_start, row_end, my_rank, world_size);
    
    // Check if all temperaute is balance
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Allreduce(&local_balance, &global_balance, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    if (global_balance) {
      break;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Update tempature table
    int *tmp = temp;
    temp = next;
    next = tmp;
  }

  // Find min tempature
  int local_min, global_min;
  MPI_Barrier(MPI_COMM_WORLD);
  local_min = findMatMin(temp, row_start, row_end);
  MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, MASTER_RANK, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);

  // Print Result
  if(my_rank == MASTER_RANK) {
    printf("Size: %d*%d, Iteration: %d, Min Temp: %d\n", L, W, count, global_min);
  }
  MPI_Finalize();
  return 0;
}