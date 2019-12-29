# Hello World
	cd hello_world
	g++ hello.cpp -o hello -lOpenCL
	./hello

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

### Speed Comparsion 
	bash comparsion.sh
	
