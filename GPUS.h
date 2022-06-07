#pragma once
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.hpp>
#include <string>
#include "VertexMesh.h"

class GPUS
{
public:
	//
	// Method for initialize GPU program
	// Initializing info about devices and platforms
	// Setting platforms, devices
	// @return err
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
	cl_context context_;

	//
	//
	//
	cl_command_queue queue_;

	//
	//
	//
	std::vector<cl_kernel> kernels_;

	//
	// if Active
	//
	bool active_ = false;

	//
	// Create program
	//
	cl::Program createProgram(const std::string&);
};