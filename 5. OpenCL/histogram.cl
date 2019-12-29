__kernel void histogram(
    __global uchar* img,
    int n_pixels,
    __global uint* hist)
{
    __local uint local_hist[256*3];
    int num_data = n_pixels;
    int lid = get_local_id(0);
    int gid = get_global_id(0);
    int local_size = get_local_size(0);
    int global_size = get_global_size(0);

    /* Init Local Histogram */
    for(int i=lid; i<256*3; i+=local_size) { 
    	local_hist[i] = 0;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    /* Calculate Local Histogram  */
    for(int i=gid; i<num_data; i+=global_size) { 
        uchar pixel_r = img[i*4 + 0]; 
        uchar pixel_g = img[i*4 + 1]; 
        uchar pixel_b = img[i*4 + 2]; 
        atom_inc(&local_hist[pixel_r + 256*0]);
        atom_inc(&local_hist[pixel_g + 256*1]);
        atom_inc(&local_hist[pixel_b + 256*2]);
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    /* Calculate Global Histogram  */
    for(int i=lid; i<256*3; i+=local_size) { 
    	atom_add(&hist[i], local_hist[i]); 
    }
}

__kernel void cal_partial_histogram(
    __global uchar* img,
    int n_pixels,
    __global uint* partial_hist)
{
    __local uint local_hist[256*3];
    int num_data = n_pixels;
    int lid = get_local_id(0);
    int gid = get_global_id(0);
    int local_size = get_local_size(0);
    int global_size = get_global_size(0);

    /* Init Local Histogram */
    for(int i=lid; i<256*3; i+=local_size) { 
    	local_hist[i] = 0;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    /* Calculate Local Histogram  */
    for(int i=gid; i<num_data; i+=global_size) { 
        uchar pixel_r = img[i*4 + 0]; 
        uchar pixel_g = img[i*4 + 1]; 
        uchar pixel_b = img[i*4 + 2]; 
        atom_inc(&local_hist[pixel_r + 256*0]);
        atom_inc(&local_hist[pixel_g + 256*1]);
        atom_inc(&local_hist[pixel_b + 256*2]);
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    /* Copy Data to Partial Histogram */
    int hist_offset = get_group_id(0) * 256 *3;
    for(int i=lid; i<256*3; i+=local_size) { 
    	partial_hist[hist_offset + i] = local_hist[i];
    }
}
__kernel void combine_partial_hist(
    __global uint *partial_hist,
    int num_group,
    __global uint *hist)
{
    int gid = get_global_id(0);
    int global_size = get_global_size(0);

    uint sum = 0;
    for(int i=0; i<num_group; i++) {
        int idx = (i*256*3) + gid; 
    	sum += partial_hist[idx];
    }
    hist[gid] = sum;
}
