#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <iostream>
#include <fstream>
#include <vector>

#include "Structs.h"

using namespace std;
using namespace cl;

class GPUFUN {
private:
	Context		 context;
	CommandQueue queue;

	std::vector<Kernel>	 kernels;
	std::vector<Platform> platforms;
	std::vector<Device>	 devices;

	int GetPlatform()
	{
		cl::Platform::get(&platforms);
		if (platforms.empty()) {
			std::cerr << "OpenCL platforms not found." << std::endl;
			return 1;
		}
		return 0;
	}
	int GetDevice()
	{
		for (auto p = platforms.begin(); devices.empty() && p != platforms.end(); p++) {
			std::vector<cl::Device> pldev;

			try {
				p->getDevices(CL_DEVICE_TYPE_GPU, &pldev);

				for (auto d = pldev.begin(); devices.empty() && d != pldev.end(); d++) {
					if (!d->getInfo<CL_DEVICE_AVAILABLE>()) continue;

					std::string ext = d->getInfo<CL_DEVICE_EXTENSIONS>();

					if (
						ext.find("cl_khr_fp64") == std::string::npos &&
						ext.find("cl_amd_fp64") == std::string::npos
						) continue;

					devices.push_back(*d);
					context = cl::Context(devices);
				}
			}
			catch (...) {
				devices.clear();
			}
		}
		if (devices.empty()) {
			std::cerr << "GPUs with double precision not found." << std::endl;
			return 1;
		}
		return 0;
	}
public:
	GPUFUN(std::vector<string>, std::vector<const char*>);
	GPUFUN();
	~GPUFUN();

	void execute();
	void execute(std::vector<face>&);
	void matrix(vector<face>&, double*);

	void   info();
	string info(bool);
};

GPUFUN::GPUFUN(std::vector<string> source_file, vector<const char*> source_function)
{
	for (int i = 0; i < source_file.size(); ++i)
	{
		try {
			//Getting all platforms
			GetPlatform();
			//Getting device with double precision extension
			GetDevice();
			// Create command queue.
			queue = cl::CommandQueue(context, devices[0]);
			// Compile OpenCL program for found device.
			ifstream helloworldfile(source_file[i]);														//File stream main.cl   --- programm for GPU
			string src(istreambuf_iterator<char>(helloworldfile),											//Text of programm
				(istreambuf_iterator<char>())
			);
			const char* source_str = src.c_str();

			Program program;
			program = cl::Program(context,
				cl::Program::Sources(1, std::make_pair(source_str, strlen(source_str)))
			);

			try {
				program.build(devices);
			}
			catch (const cl::Error&) {
				std::cerr
					<< "OpenCL compilation error" << std::endl
					<< program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0])
					<< std::endl;
			}

			cl::Kernel kern(program, source_function[i]);
			kernels.push_back(kern);
		}
		catch (const cl::Error& err) {
			std::cerr
				<< "OpenCL error: "
				<< err.what() << "(" << err.err() << ")"
				<< std::endl;
			printf("Troubles with Device!");
		}
	}

	//cl::Buffer A(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, a.size() * sizeof(double), a.data());
	//kernel.setArg(0, A);
}
GPUFUN::GPUFUN() 
{
	try {
		//Getting all platforms
		GetPlatform();
		//Getting device with double precision extension
		GetDevice();
	}
	catch (const cl::Error& err) {
		std::cerr
			<< "OpenCL error: "
			<< err.what() << "(" << err.err() << ")"
			<< std::endl;
		printf("Troubles with Device!");
	}
}
GPUFUN::~GPUFUN() {}
void GPUFUN::execute()
{
	queue.enqueueNDRangeKernel(kernels[0], cl::NullRange, 1, cl::NullRange);
}
void GPUFUN::execute(std::vector<face>& f)
{
	int size = sizeof(int) * VPOS_COUNT * FVEC_COUNT * 3 * f.size();
	if (size) {
		cl::Buffer FACE(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size, f.data());
		kernels[1].setArg(0, FACE);
		kernels[1].setArg(1, FVEC_COUNT);
		kernels[1].setArg(2, VPOS_COUNT);
		queue.enqueueNDRangeKernel(kernels[1], cl::NullRange, cl::NDRange(f.size()), cl::NullRange);
		queue.enqueueReadBuffer(FACE, 0, 0, size, f.data(), 0, NULL);
	}
}

//Matrix multiplication sending data to GPU
void GPUFUN::matrix(std::vector<face>& f, double* matrix)
{
	int size = sizeof(int) * VPOS_COUNT * FVEC_COUNT * 3 * f.size();
	if (size) {
		cl::Buffer FACE(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size, f.data());
		cl::Buffer MATRIX(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, MATRIX_COUNT, matrix);
		kernels[2].setArg(0, FACE);
		kernels[2].setArg(1, FVEC_COUNT);
		kernels[2].setArg(2, VPOS_COUNT);
		kernels[2].setArg(3, MATRIX);
		kernels[2].setArg(4, SCALE_POSES);

		queue.enqueueNDRangeKernel(kernels[2], cl::NullRange, cl::NDRange(f.size()), cl::NullRange);
		queue.enqueueReadBuffer(FACE, 0, 0, size, f.data(), 0, NULL);
	}
}

void GPUFUN::info()
{
	printf(devices[0].getInfo<CL_DEVICE_VENDOR>().c_str());			   printf("\n");
	printf(devices[0].getInfo<CL_DEVICE_NAME>().c_str());			   printf("\n");
	try { printf("%d", int(devices[0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>())); }
	catch (const cl::Error& err) { printf("%d", err.err()); }
	printf("\n");
	printf("%d", devices[0].getInfo<CL_DEVICE_AVAILABLE>());		   printf("\n");
}
string GPUFUN::info(bool extended)
{
	string areturn = "";
	if (extended) {
		areturn += (devices[0].getInfo<CL_DEVICE_VENDOR>() + " ").c_str();
		areturn += (devices[0].getInfo<CL_DEVICE_NAME>() + " ").c_str();
		areturn += std::to_string(devices[0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>()) + " ";
		if (devices[0].getInfo<CL_DEVICE_AVAILABLE>()) {
			areturn += "V";
		}
		else areturn += "X";
	}
	else {
		areturn += std::to_string(devices[0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>()) + " ";
		if (devices[0].getInfo<CL_DEVICE_AVAILABLE>()) {
			areturn += "V";
		}
		else areturn += "X";
	}

	return areturn;
}