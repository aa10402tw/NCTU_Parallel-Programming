# Hello World
	g++ hello_world/hello.cpp -o hello_world/hello -lOpenCL
	./hello_world/hello
# Image Histogram
### Serial Version
	g++ serial_histogram.cpp -o serial_histogram
	./serial_histogram airplane.bmp
	mv hist_airplane.bmp serial_hist_airplane.bmp
### OpenCL Version
	g++ opencl_histogram.cpp -o opencl_histogram -lOpenCL
	./opencl_histogram airplane.bmp
	mv hist_airplane.bmp opencl_hist_airplane.bmp
	python check.py --img_name="airplane"

### Comparsion
	bash comparsion.sh
	
