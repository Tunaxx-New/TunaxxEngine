#pragma once
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.hpp>
#include <string>
#include "VertexMesh.h"

class GPU
{
	struct platform
	{
		cl::Platform pform;
		std::vector<cl::Device> devices;
	};
public:
	//
	// Method for initialize GPU program
	// Initializing info about devices and platforms
	// Setting platforms, devices
	// @return
	// 0 - all is ok
	// 1 - List of platforms is empty
	// 2 - List of devices is empty
	// 3 - program building error
	// 4 - some CL error
	int initialize();

	//
	// Clear the array
	//
	void execute(COLORREF**, int);

	//
	// Executes overloaded execute functions to OpenCL program and kernel
	// This function blitting random pixels to bitmap
	// @param1 bitmap
	// @param2 vector of bits
	void execute(COLORREF**, std::vector<unsigned int>);

	//
	// Executes overloaded execute functions to OpenCL program and kernel
	// This function executes types of fun programs
	// @param1 bitmap
	// @param2 size of bitmap
	// @param3 height and width of bitmap
	// @param4 type of function
	// @param5 rparam
	// @param6 iparam
	//   0 - Mandelbrot set
	//   1 - Conway's Game of Life
	void execute(COLORREF**, int, COORD, char, float, float);

	// This function blitting scene and matrix object
	// @param1 bitmap
	// @param2 buffer size
	// @param3 size of window
	// @param4 object with faces
	// @param5 matrixes
	void execute(COLORREF**, int, COORD, object<cl_float>, std::vector<std::vector<float>>);

	//
	// Method to get info_ string
	//
	std::string& getInfo();

	//
	// Method to set active
	//
	void setActive();

	//
	// Method to get active
	//
	bool getActive();

	//
	//
	//
	void release();
private:
	//
	// Info about all platforms, devices
	//
	std::string info_ = "";

	//
	//
	//
	cl::Context context_;

	//
	//
	//
	cl::CommandQueue queue_;

	//
	//
	//
	std::vector<cl::Kernel> kernels_;

	//
	//
	//
	std::vector<platform> platforms_;

	//
	// Current platform and device
	// Y - Platform
	// X - Device
	COORD currentPD_ = { -1, 0 };

	//
	// if Active
	//
	bool active_ = false;

	//
	// Max item size - 1
	//
	std::vector<size_t> maxItemSize_ = {0, 0, 0};
};

//
// This function read file from path
// @return file source code of kernel .cl
const char* kernelSource(std::string);