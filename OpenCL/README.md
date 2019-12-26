# Hello World
	g++ hello.cpp -o hello -lOpenCL
	./hello
# Image Histogram
### Serial Version
	g++ serial_histogram.cpp -o serial_histogram
	./serial_histogram small.bmp
### OpenCL Version
	g++ opencl_histogram.cpp -o opencl_histogram -lOpenCL
	./opencl_histogram small.bmp
	python check.py
