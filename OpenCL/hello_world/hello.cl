__kernel 
void vecadd(__global float* A, 
            __global float* B, 
            __global float* C)
{
    const int tid = get_global_id(0);
    C[tid] = A[tid] + B[tid];
}