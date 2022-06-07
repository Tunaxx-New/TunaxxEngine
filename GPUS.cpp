#define __CL_ENABLE_EXCEPTIONS
#include "GPUS.h"
#include "Constants.h"
#include <iostream>
#include <fstream>
#include <codecvt>
#include <iomanip>
#include <fstream>



int GPUS::initialize()
{
	cl_platform_id platform_id = NULL;
	cl_device_id device_id     = NULL;
	cl_uint platforms_count;
	cl_uint devices_count;

	cl_int ret = clGetPlatformIDs(1, &platform_id, &platforms_count);
	ret |= clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &devices_count);

	cl_context context             = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/*cl::Program program = CreateProgram("array.cl");
	auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
	auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
	auto& device = devices.front();

	cl::Kernel kernel(program, "array");
	cl_int err = 0;
	int WorkGroupSize = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device, &err);

	WorkGroupSize = 2;
	//cout << WorkGroupSize << '\n';
	int NumWorkGroup = f.v.size() / WorkGroupSize;

	cl::Buffer inBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * f.v.size(), f.v.data());
	cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * NumWorkGroup);

	kernel.setArg(0, inBuf);
	kernel.setArg(1, sizeof(int) * WorkGroupSize, nullptr);
	kernel.setArg(2, outBuf);

	vector<int> outVec(f.v.size());

	cl::CommandQueue queue(context, device);
	queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(f.v.size()), cl::NDRange(WorkGroupSize));
	queue.enqueueReadBuffer(outBuf, GL_TRUE, 0, sizeof(int) * outVec.size(), outVec.data());*/

	return 0;
}

cl::Program createProgram(const std::string& file) {
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	auto device = devices.front();
	auto vendor = device.getInfo<CL_DEVICE_VENDOR>();
	auto version = device.getInfo<CL_DEVICE_VERSION>();
																									//
	std::ifstream helloworldfile(file);
	std::string src(std::istreambuf_iterator<char>(helloworldfile), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));
	cl::Context context(device);
	cl::Program program(context, sources);

	program.build("-cl-std=CL1.2");
	return program;
}