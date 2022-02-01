#pragma once
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.hpp>
#include <string>
#include "Structs.h"

using namespace std;

class GPUFUN {
private:
	cl::Context		 context;
	cl::CommandQueue queue;

	std::vector<cl::Context> contexts;
	vector<cl::Kernel>	 kernels;
	vector<cl::Platform> platforms;
	vector<cl::Device>	 devices;

	int GetPlatform();
	int GetDevice();
public:
	GPUFUN(vector<string> source, vector<const char*> function);
	GPUFUN();
	~GPUFUN();

	void execute();
	void execute(std::vector<face>&);
	void matrix(vector<face>&, double*);

	void   info();
	string info(bool);
};