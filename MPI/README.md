# Compile file 
mpicc -o mpi_hello_world mpi_hello_world.c 

# Execute file
mpiexec -n 4 mpi_hello_world

