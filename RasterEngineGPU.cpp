#include "RasterEngineGPU.h"
#include "VertexMesh.h"
#include <iostream>
#include <random>


void RasterEngineGPU::initialize()
{
	std::cout << gpu_.initialize();
	Mesh<cl_float> cube;
	scene_.push_back(cube);
	gpu_.setActive();
}



void RasterEngineGPU::gswitch()
{
	gpu_.setActive();
}



//
// CODE FOR:
// Random noise blitting:
/*
std::vector<unsigned int> randomPixels(size, 0);
randomVector(randomPixels, 0, 0xFFFFFF);
gpu_.execute(bitmap, randomPixels);
*/
// Mandelbrot set:
/*
gpu_.execute(bitmap, size, sizeb, 0, rparam_, iparam_);
	if (switcher_) {
		rparam_ += 0.01f;
		iparam_ += 0.01f;
		if (rparam_ > 1000)
			switcher_ = false;
	}
	else {
		rparam_ -= 0.01f;
		iparam_ -= 0.001f;
		if (rparam_ < 0)
			switcher_ = true;
	}
*/
// Conway's Game of Life
/*
gpu_.execute(bitmap, size, sizeb, 1, rparam_, iparam_);
*/
void RasterEngineGPU::draw(COLORREF** bitmap, int size, COORD sizeb, int type)
{
	/*memset(*bitmap, 0, size * sizeof(COLORREF));

	for (Mesh<cl_float>& mesh : scene_) {
		mesh.rotate(1, 1, 1);
		gpu_.execute(bitmap, size, sizeb, mesh.getObject(), mesh.getMatrixes());
	}*/
	switch (type)
	{
	case 0:
	{
		gpu_.execute(bitmap, size, sizeb, 0, rparam_, iparam_);
		if (switcher_) {
			rparam_ += 0.1f;
			iparam_ += 0.1f;
			if (rparam_ > 1000)
				switcher_ = false;
		}
		else {
			rparam_ -= 0.01f;
			iparam_ -= 0.001f;
			if (rparam_ < 0)
				switcher_ = true;
		}
		break;
	}
	
	case 1:
		gpu_.execute(bitmap, size, sizeb, 1, rparam_, iparam_);
		break;
	}
}



void RasterEngineGPU::release()
{
	gpu_.release();
}



std::string RasterEngineGPU::getGPUInfo()
{
	return gpu_.getInfo();
}



template <typename T>
void RasterEngineGPU::randomVector(std::vector<T>& vec, int start, int finish)
{
	std::random_device randomDevice;
	std::mt19937 mersenneEngine{ randomDevice() };
	std::uniform_int_distribution<T> dist{ start, finish };
	auto gen = [&dist, &mersenneEngine]() {
		return dist(mersenneEngine);
	};
	generate(begin(vec), end(vec), gen);
}

template<>
void RasterEngineGPU::randomVector(std::vector<unsigned int>& vec, int start, int finish)
{
	std::random_device randomDevice;
	std::mt19937 mersenneEngine{ randomDevice() };
	std::uniform_int_distribution<int64_t> dist{ start, finish };
	auto gen = [&dist, &mersenneEngine]() {
		return dist(mersenneEngine);
	};
	generate(begin(vec), end(vec), gen);
}