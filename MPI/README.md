# Hello World
## In single computer
	mpicc -o mpi_hello_world mpi_hello_world.c 
	mpiexec -n 4 mpi_hello_world

## In NFS server
	/home/PP-f19/MPI/bin/mpicc mpi_hello_world.c -o mpi_hello_world
	/home/PP-f19/MPI/bin/mpiexec -npernode 1 --hostfile hostfile mpi_hello_world
	
# Tempature Conduction
### In single computer
	mpicc -o conduction conduction.c
	mpiexec -n 1 conduction 20 100 0
	mpiexec -n 4 conduction 20 100 0

### In NFS server
	/home/PP-f19/MPI/bin/mpicc conduction.c -o conduction
	/home/PP-f19/MPI/bin/mpiexec -npernode 1 --hostfile hostfile -n 1 conduction 50000 100 0
	/home/PP-f19/MPI/bin/mpiexec -npernode 1 --hostfile hostfile -n 4 conduction 50000 100 0
