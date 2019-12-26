#include <fstream>
#include <iostream>
#include <string>
#include <ios>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <CL/cl.h>

typedef struct
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t align;
} RGB;

typedef struct
{
    bool type;
    uint32_t size;
    uint32_t height;
    uint32_t weight;
    RGB *data;
} Image;

Image *readbmp(const char *filename)
{
    std::ifstream bmp(filename, std::ios::binary);
    char header[54];
    bmp.read(header, 54);
    uint32_t size = *(int *)&header[2];
    uint32_t offset = *(int *)&header[10];
    uint32_t w = *(int *)&header[18];
    uint32_t h = *(int *)&header[22];
    uint16_t depth = *(uint16_t *)&header[28];
    if (depth != 24 && depth != 32)
    {
        printf("we don't suppot depth with %d\n", depth);
        exit(0);
    }
    bmp.seekg(offset, bmp.beg);

    Image *ret = new Image();
    ret->type = 1;
    ret->height = h;
    ret->weight = w;
    ret->size = w * h;
    ret->data = new RGB[w * h]{};
    for (int i = 0; i < ret->size; i++)
    {
        bmp.read((char *)&ret->data[i], depth / 8);
    }
    return ret;
}

int writebmp(const char *filename, Image *img)
{

    uint8_t header[54] = {
        0x42,        // identity : B
        0x4d,        // identity : M
        0, 0, 0, 0,  // file size
        0, 0,        // reserved1
        0, 0,        // reserved2
        54, 0, 0, 0, // RGB data offset
        40, 0, 0, 0, // struct BITMAPINFOHEADER size
        0, 0, 0, 0,  // bmp width
        0, 0, 0, 0,  // bmp height
        1, 0,        // planes
        32, 0,       // bit per pixel
        0, 0, 0, 0,  // compression
        0, 0, 0, 0,  // data size
        0, 0, 0, 0,  // h resolution
        0, 0, 0, 0,  // v resolution
        0, 0, 0, 0,  // used colors
        0, 0, 0, 0   // important colors
    };

    // file size
    uint32_t file_size = img->size * 4 + 54;
    header[2] = (unsigned char)(file_size & 0x000000ff);
    header[3] = (file_size >> 8) & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;

    // width
    uint32_t width = img->weight;
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8) & 0x000000ff;
    header[20] = (width >> 16) & 0x000000ff;
    header[21] = (width >> 24) & 0x000000ff;

    // height
    uint32_t height = img->height;
    header[22] = height & 0x000000ff;
    header[23] = (height >> 8) & 0x000000ff;
    header[24] = (height >> 16) & 0x000000ff;
    header[25] = (height >> 24) & 0x000000ff;

    std::ofstream fout;
    fout.open(filename, std::ios::binary);
    fout.write((char *)header, 54);
    fout.write((char *)img->data, img->size * 4);
    fout.close();
}

cl_program load_program(cl_context context, const char* filename)
{
    std::ifstream in(filename, std::ios_base::binary);
    if(!in.good()) {
        return 0;
    }

    // get file length
    in.seekg(0, std::ios_base::end);
    size_t length = in.tellg();
    in.seekg(0, std::ios_base::beg);

    // read program source
    std::vector<char> data(length + 1);
    in.read(&data[0], length);
    data[length] = 0;

    // create and build program 
    const char* source = &data[0];
    cl_program program = clCreateProgramWithSource(context, 1, &source, 0, 0);
    if(program == 0) {
        return 0;
    }

    if(clBuildProgram(program, 0, 0, 0, 0, 0) != CL_SUCCESS) {
        return 0;
    }

    return program;
}

double executionTime(cl_event &event){
    cl_ulong start, end;

    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);

    return (double)1.0e-9 * (end - start); // convert nanoseconds to seconds on return
}

int main(int argc, char *argv[])
{
    cl_int clError;
    cl_uint num_device, ret_num_platforms;
    cl_device_id device;
    cl_platform_id platform_id;
    
    /* Get OpenCL Platform */
    clError = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    if(clError != CL_SUCCESS) {
        printf("clGetPlatformIDs fails (%d)\n", clError);
        return -1;
    }
    /* Get OpenCL Device */
    clError = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device, &num_device);
    if(clError != CL_SUCCESS) {
        printf("clGetDeviceIDs fails (%d)\n", clError);
        return -1;
    }
    
    /* Get OpenCL Context */
    cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform_id), 0 };
    cl_context myctx = clCreateContext(prop, 1, &device, NULL, NULL, &clError);
    if(!myctx || clError != CL_SUCCESS) {
        printf("clCreateContext fails (%d)\n", clError);
        return -1;
    }
    
    /* Get OpenCL Command Queue */
    cl_command_queue myque = clCreateCommandQueueWithProperties(myctx, device, 0, &clError);
    if(clError != CL_SUCCESS) {
        printf("clCreateCommandQueueWithProperties fails (%d)\n", clError);
        return -1;
    }
    
    /* Do image histogram to each input image path */
    char *filename;
    if (argc >= 2)
    {
        int many_img = argc - 1;
        for (int i = 0; i < many_img; i++)
        {
            uint32_t R[256], G[256], B[256];
            filename = argv[i + 1];

            /* Read Bmp image */
            Image *img = readbmp(filename);
            std::cout << img->weight << ":" << img->height << "\n";

            int n_pixels = img->height * img->weight;
            size_t img_size  = sizeof(uint) * n_pixels;
            size_t hist_size = sizeof(uint) * 256 * 3;
            int num_group = 128;
            size_t partial_hist_size = hist_size * num_group;
            
            /* Create OpenCL Buffer */ 
            cl_mem cl_img = clCreateBuffer(myctx, CL_MEM_READ_ONLY,   img_size,  NULL, &clError);
            cl_mem cl_his = clCreateBuffer(myctx, CL_MEM_WRITE_ONLY,  hist_size, NULL, &clError);
            cl_mem cl_partial_his = clCreateBuffer(myctx, CL_MEM_WRITE_ONLY,  partial_hist_size, NULL, &clError);
            uint32_t result_his[256*3];
            memset(result_his, 0, hist_size);
            clError |= clEnqueueWriteBuffer(myque, cl_his, CL_TRUE, 0, hist_size, result_his, 0, NULL, NULL);
            clError |= clEnqueueWriteBuffer(myque, cl_img, CL_TRUE, 0, img_size,  img->data,  0, NULL, NULL);
            if(clError != CL_SUCCESS) {
                printf("clCreateBuffer fails (%d)\n", clError);
                return -1;
            }

            /* Load OpenCL Program */
            cl_program myprog = load_program(myctx, "histogram.cl");
            clError = clBuildProgram(myprog, 1, &device, NULL, NULL, NULL);
            if(clError != CL_SUCCESS) {
                printf("clBuildProgram fails (%d)\n", clError);
                return -1;
            }
            
            /* Create OpenCL Kernel (cal_partial_histogram) */
            cl_kernel kernel_cal_partial = clCreateKernel(myprog, "cal_partial_histogram", &clError);
            clError |= clSetKernelArg(kernel_cal_partial, 0, sizeof(cl_mem), &cl_img);
            clError |= clSetKernelArg(kernel_cal_partial, 1, sizeof(int), &n_pixels);
            clError |= clSetKernelArg(kernel_cal_partial, 2, sizeof(cl_mem), &cl_partial_his);
            if(clError != CL_SUCCESS) {
                printf("clCreateKernel partial fails (%d)\n", clError);
                return -1;
            }

            /* Create OpenCL Kernel Kernel */
            cl_kernel kernel_comb_partial = clCreateKernel(myprog, "combine_partial_hist", &clError);
            clError |= clSetKernelArg(kernel_comb_partial, 0, sizeof(cl_mem), &cl_partial_his);
            clError |= clSetKernelArg(kernel_comb_partial, 1, sizeof(int), &num_group);
            clError |= clSetKernelArg(kernel_comb_partial, 2, sizeof(cl_mem), &cl_his);
            if(clError != CL_SUCCESS) {
                printf("clCreateKernel partial fails (%d)\n", clError);
                return -1;
            }
            
            /* Execute OpenCL Kernel (kernel_cal_partial) */
            size_t local_work_size  = 256;
            size_t global_work_size = local_work_size * num_group;
            
            clError = clEnqueueNDRangeKernel(myque, kernel_cal_partial, 1, 0, &global_work_size, &local_work_size, 0, NULL, NULL);
            if(clError != CL_SUCCESS) {
                printf("clEnqueueNDRangeKernel cal_partial fails (%d)\n", clError);
                return -1;
            }
            /* Execute OpenCL Kernel (kernel_comb_partial) */
            size_t partial_global_work_size = 256*3;
            size_t partial_local_work_size  = 256;
            clError = clEnqueueNDRangeKernel(myque, kernel_comb_partial, 1, 0, &partial_global_work_size, &partial_local_work_size, 0, NULL, NULL);
            if(clError != CL_SUCCESS) {
                printf("clEnqueueNDRangeKernel comb_partial fails (%d)\n", clError);
                return -1;
            }
            /* Read Result from OpenCL Buffer */
            clError = clEnqueueReadBuffer(myque, cl_his, CL_TRUE, 0, hist_size, result_his, 0, NULL, NULL);
            if(clError != CL_SUCCESS) {
                printf("clEnqueueReadBuffer fails (%d)\n", clError);
                return -1;
            }

            /* Convert OpenCL Program Result */
            for(int i=0; i<256; i++){
                R[i] = result_his[i + 256*0];
                G[i] = result_his[i + 256*1];
                B[i] = result_his[i + 256*2];
            }
            
            /* Write Bmp */
            int max = 0;
            for(int i=0;i<256;i++){
                max = R[i] > max ? R[i] : max;
                max = G[i] > max ? G[i] : max;
                max = B[i] > max ? B[i] : max;
            }

            Image *ret = new Image();
            ret->type = 1;
            ret->height = 256;
            ret->weight = 256;
            ret->size = 256 * 256;
            ret->data = new RGB[256 * 256];
            
            for(int i=0;i<ret->height;i++){
                for(int j=0;j<256;j++){
                    ret->data[256*i+j].R = 0;
                    ret->data[256*i+j].G = 0;
                    ret->data[256*i+j].B = 0;
                }
            }

            for(int i=0;i<ret->height;i++){
                for(int j=0;j<256;j++){
                    if(R[j]*256/max > i)
                        ret->data[256*i+j].R = 255;
                    if(G[j]*256/max > i)
                        ret->data[256*i+j].G = 255;
                    if(B[j]*256/max > i)
                        ret->data[256*i+j].B = 255;
                }
            }
            std::string newfile = "hist_" + std::string(filename); 
            writebmp(newfile.c_str(), ret);
        }
    }else{
        printf("Usage: ./hist <img.bmp> [img2.bmp ...]\n");
    }
    return 0;
}

