# Hello World
	mpicc -o mpi_hello_world mpi_hello_world.c 
	mpiexec -n 4 mpi_hello_world
	
# Tempature Conduction
	mpicc -o conduction conduction.c
	mpiexec -n 4 conduction 20 100 0

