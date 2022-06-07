#define __CL_ENABLE_EXCEPTIONS
#include "GPU.h"
#include "Constants.h"
#include <iostream>
#include <fstream>
#include <codecvt>
#include <iomanip>


int GPU::initialize()
{
	try {
		//Some near local variables
		std::vector<cl::Platform> platforms;

		//Getting platforms, checking emptiness, reading info
		cl::Platform::get(&platforms);
		if (platforms.empty())
			return 1;

		info_ += "PLATFORMS: ";
		info_ += NEW_LINE;

		//
		// Prioritize to COORD currentPD_
		//

		int i = 1;
		for (auto p = platforms.begin(); p != platforms.end(); p++) {
			//Getting info about platform
			info_ += std::to_string(i) + ". ";
			info_ += "Name: " + (*p).getInfo<CL_PLATFORM_NAME>() + NEW_LINE;
			info_ += "Extensions: " + (*p).getInfo<CL_PLATFORM_EXTENSIONS>() + NEW_LINE;
			info_ += "Profile: " + (*p).getInfo<CL_PLATFORM_PROFILE>() + NEW_LINE;
			info_ += "Vendor: " + (*p).getInfo<CL_PLATFORM_VENDOR>() + NEW_LINE;
			info_ += "Version: " + (*p).getInfo<CL_PLATFORM_VERSION>() + NEW_LINE;

			platform temp;
			temp.pform = (*p);

			//Getting devices
			(*p).getDevices(CL_DEVICE_TYPE_GPU, &temp.devices);
			if (temp.devices.empty())
				return 2;

			if (currentPD_.X == -1)
				currentPD_.Y = i - 1;

			info_ += "AVAILABEL DEVICES: ";
			info_ += NEW_LINE;
			int j = 1;
			for (auto d = temp.devices.begin(); d != temp.devices.end(); d++) {
				//If device available
				if (!(*d).getInfo<CL_DEVICE_AVAILABLE>()) continue;


				//Main device info------------------------
				std::string extensions = (*d).getInfo<CL_DEVICE_EXTENSIONS>();
				info_ += std::to_string(j) + ". ";
				info_ += "Name: " + (*d).getInfo<CL_DEVICE_NAME>() + NEW_LINE;
				info_ += "Extensions: " + extensions + NEW_LINE;
				info_ += "Profile: " + (*d).getInfo<CL_DEVICE_PROFILE>() + NEW_LINE;
				info_ += "Vendor: " + (*d).getInfo<CL_DEVICE_VENDOR>() + NEW_LINE;
				info_ += "Device version: " + (*d).getInfo<CL_DEVICE_VERSION>() + NEW_LINE;
				info_ += "Driver version: " + (*d).getInfo<CL_DRIVER_VERSION>() + NEW_LINE;
				info_ += "OPENCL C version: " + (*d).getInfo<CL_DEVICE_OPENCL_C_VERSION>() + NEW_LINE;
				info_ += "Kernels: " + (*d).getInfo<CL_DEVICE_BUILT_IN_KERNELS>() + NEW_LINE;
				info_ += "Max work item sizes: ";
				std::vector<size_t> maxItemSizes = (*d).getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();


				for (auto size = maxItemSizes.begin(); size != maxItemSizes.end(); size++) {
					info_ += std::to_string((int)(*size));
					info_ += ' ';
				}
				info_ += NEW_LINE;
				//----------------------------------------

				//Availability to double percussion--------
				bool khr64 = extensions.find("cl_khr_fp64") != std::string::npos;
				bool amd64 = extensions.find("cl_amd_fp64") != std::string::npos;
				if (khr64 || amd64) {
					info_ += "Double percussion is available";
					currentPD_.X = j - 1;
				}
				else {
					info_ += "Double percussion is not available";
				}
				info_ += NEW_LINE;
				//-----------------------------------------

				info_ += NEW_LINE;
				j++;
			}
			i++;
			platforms_.push_back(temp);
		}

		if (currentPD_.X == -1)
			currentPD_.X = 0;

		
		platforms_ = { platforms_[currentPD_.Y] };
		platforms_[0].devices = { platforms_[0].devices[currentPD_.X] };
		context_ = cl::Context(platforms_[currentPD_.Y].devices);
		
		maxItemSize_ = platforms_[0].devices[0].getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
		for (auto& item : maxItemSize_)
			item--;

		std::string file = FILE_CL;
		std::vector<std::string> commands = METHODS_PAINT_CL;
		
		const char* source = kernelSource(file);
		cl::Program program;
		cl::Program::Sources sources(1, std::make_pair(source, strlen(source) + 1));
		int err = 0;
		program = cl::Program(context_, sources);


		/*cl_platform_id platform_id = NULL;
		cl_device_id device_id = NULL;
		cl_uint ret_num_devices;
		cl_uint ret_num_platforms;
		cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
		ret |= clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
		cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);


		cl_program programa;*/
		try {
			//program.build({ platforms_[0].devices[0] });
			//program.compile("-cl-std=CL1.2");
			program.compile("-D GPU_FORCE_64BIT_PTR=1 -D GPU_USE_SYNC_OBJECTS=1 -D GPU_MAX_ALLOC_PERCENT=100");
			program.build(platforms_[0].devices, "-D GPU_FORCE_64BIT_PTR=1 -D GPU_USE_SYNC_OBJECTS=1 -D GPU_MAX_ALLOC_PERCENT=100");
			//const size_t a = strlen(source) + 1;
			//programa = clCreateProgramWithSource(context, 1, &source, &a, NULL);
		}
		catch (const cl::Error&) {
			std::cerr
				<< "OpenCL compilation error\n"
				<< program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(platforms_[0].devices[0])
				<< '\n';
			return 3;
		}
		program.createKernels(&kernels_);
		//for (auto& cmd : commands)
		//{
		//	cl::Kernel kernel = program.createKernels(kernels_);
		//	kernels_.push_back(kernel);
		//}
		queue_ = cl::CommandQueue(context_, platforms_[currentPD_.Y].devices[currentPD_.X], CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
	}
	catch (const cl::Error& err) {
		std::cerr
			<< "OpenCL error: "
			<< err.what() << '(' << err.err() << ")\n";
		return 4;
	}
	return 0;
}


/*
void GPU::execute(COLORREF** bitmap, int bitmapSize)
{
	if (active_) {
		int size = bitmapSize * sizeof((*bitmap)[0]);
		cl::Buffer BITMAP(context_, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, size, (*bitmap));
		kernels_[4].setArg(0, BITMAP);
		queue_.enqueueNDRangeKernel(kernels_[4], cl::NullRange, cl::NDRange(bitmapSize), cl::NullRange);
		queue_.finish();
	}
}



void GPU::execute(COLORREF** bitmap, std::vector<unsigned int> randomPixels)
{	
	if (active_) {
		int size = randomPixels.size() * sizeof(unsigned int);
		cl::Buffer BITMAP(context_, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, size, (*bitmap));
		cl::Buffer MATRIX(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size, randomPixels.data());
		kernels_[0].setArg(0, BITMAP);
		kernels_[0].setArg(1, MATRIX);
		queue_.enqueueNDRangeKernel(kernels_[0], cl::NullRange, cl::NDRange(randomPixels.size()), cl::NullRange);
		queue_.finish();
	}
}


*/

void GPU::execute(COLORREF** bitmap, int bitmapSize, COORD sizeb, char type, float rparam, float iparam)
{
	if (active_) {
		switch (type)
		{
		case 0: {
			try {
				int size = bitmapSize * sizeof((*bitmap)[0]);
				size_t func = type;
				cl::Buffer BITMAP(context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size, (*bitmap));

				kernels_[func].setArg(0, BITMAP);
				kernels_[func].setArg(1, sizeb.X);
				kernels_[func].setArg(2, sizeb.Y);
				kernels_[func].setArg(3, rparam);
				kernels_[func].setArg(4, iparam);
				kernels_[func].setArg(5, 1024);
				queue_.enqueueNDRangeKernel(kernels_[func], cl::NullRange, cl::NDRange(1024), cl::NullRange);
				queue_.finish();
			}
			catch (const cl::Error& err) {
				std::cerr
					<< "OpenCL error: "
					<< err.what() << '(' << err.err() << ")\n";
			}
			break;
		}

		case 1: {
			try {
				int size = bitmapSize * sizeof((*bitmap)[0]);
				//cl::CommandQueue queue_ = cl::CommandQueue(context_, platforms_[currentPD_.Y].devices[currentPD_.X]);
				//cl::CommandQueue queue_ = cl::CommandQueue(CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
				size_t func = type;
				std::vector<unsigned int> newBitmap(bitmapSize, 0);
				cl::Buffer BITMAP(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size, (*bitmap));
				cl::Buffer NEWBITMAP(context_, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, size, newBitmap.data());

				kernels_[func].setArg(0, BITMAP);
				kernels_[func].setArg(1, sizeb.X);
				kernels_[func].setArg(2, sizeb.Y);
				kernels_[func].setArg(3, NEWBITMAP);
				kernels_[func].setArg(4, 1024);

				//for (int i = 0; i < size / 1024; i++) {
				//	queue_.enqueueNDRangeKernel(kernels_[func], cl::NDRange(i * 1024), cl::NDRange(1024), cl::NullRange);
				//	queue_.finish();
				//}
				queue_.enqueueNDRangeKernel(kernels_[func], cl::NullRange, cl::NDRange(1024), cl::NullRange);
				queue_.enqueueReadBuffer(NEWBITMAP, CL_TRUE, 0, size, (*bitmap), 0, NULL);
				queue_.finish();
				queue_.enqueueBarrierWithWaitList();
			}
				catch (const cl::Error& err) {
					std::cerr
						<< "OpenCL error: "
						<< err.what() << '(' << err.err() << ")\n";
				}
			break;
		}
		default:
			break;
		}
	}
}

/*

void GPU::execute(COLORREF** bitmap, int bitmapSize, COORD sizeb, object<cl_float> obj, std::vector<std::vector<float>> matrixes)
{
	if (active_) {
		//Matrixes multiplication
		std::vector<vertex<cl_float>> vertexMatrix = obj.vertexes;

		for (int i = 0; i < matrixes.size(); i++) {
			cl::Buffer VERTEXES(context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(obj.vertexes), obj.vertexes.data());
			cl::Buffer VERTEXESRESULT(context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(vertexMatrix), vertexMatrix.data());
			cl::Buffer MATRIX(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(matrixes[i]), matrixes[i].data());

			kernels_[3].setArg(0, VERTEXES);
			kernels_[3].setArg(1, MATRIX);
			kernels_[3].setArg(2, VERTEXESRESULT);
			queue_.enqueueNDRangeKernel(kernels_[3], cl::NullRange, cl::NDRange(obj.vertexes.size()), cl::NullRange);
			queue_.finish();

			obj.vertexes = vertexMatrix;
		}

		int size = bitmapSize * sizeof((*bitmap)[0]);
		cl::Buffer BITMAP(context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, size, (*bitmap));
		cl::Buffer VERTEXES(context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(obj.vertexes), obj.vertexes.data());
		cl::Buffer FACES(context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(obj.vfaces), obj.vfaces.data());

		kernels_[4].setArg(0, BITMAP);
		kernels_[4].setArg(1, sizeb.X);
		kernels_[4].setArg(2, sizeb.Y);
		kernels_[4].setArg(3, bitmapSize);
		kernels_[4].setArg(4, VERTEXES);
		kernels_[4].setArg(5, FACES);
		queue_.enqueueNDRangeKernel(kernels_[4], cl::NullRange, cl::NDRange(obj.vfaces.size()), cl::NullRange);

		queue_.finish();
	}
}

*/

std::string& GPU::getInfo()
{
	return info_;
}



void GPU::setActive()
{
	active_ = !active_;
}



bool GPU::getActive()
{
	return active_;
}



void GPU::release()
{
	queue_.flush();
	queue_.finish();
	context_.~Context();
	platforms_.clear();
	kernels_.clear();
	std::cout << "Finished GPU\n";
}



const char* kernelSource(std::string file)
{
	//https://stackoverflow.com/questions/29121443/read-opencl-kernel-from-seperate-file
	FILE* fp;
	char* source_str;
	size_t source_size, program_size;

	fopen_s(&fp, file.c_str(), "rb");
	if (!fp) {
		printf("Failed to load kernel\n");
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	program_size = ftell(fp);
	rewind(fp);
	source_str = (char*)malloc(program_size + 1);
	if (source_str) {
		source_str[program_size] = '\0';
		fread(source_str, sizeof(char), program_size, fp);
	}
	fclose(fp);
	return source_str;
}