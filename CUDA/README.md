# CUDA example : Vector Add 
    nvcc cuda_vector_add.cu -o cuda_vector_add
    cuda_vector_add.exe

# Homework Serial Version
    gcc serial_wave.c -o serial_wave -lm
    ./serial_wave.exe 25 100

# Homework CUDA Version
    nvcc cuda_wave.cu -o cuda_wave
    ./cuda_wave 25 100
