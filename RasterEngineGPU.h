#pragma once
#include "Engine.h"
#include "GPU.h"

class RasterEngineGPU : public Engine
{
public:
	//
	// Method for initializion the engine
	//
	void initialize();

	//
	// Method for drawing in bitmapBack_ from main window
	// @params COLORREF** reference to array of pixels;
	void draw(COLORREF**, int, COORD);

	//
	// Method for release the engine
	//
	void release();

	//
	// Method for release the engine
	//
	std::string getGPUInfo();

	//
	// Method for release the engine
	//
	template <typename T>
	void randomVector(std::vector<T>&, int, int);

	template <>
	void randomVector(std::vector<unsigned int>&, int, int);
private:
	// Add-----------------------
	// Real parameter
	//
	float rparam_ = 0;

	//
	// Imaginary parameter
	//
	float iparam_ = 0;

	//
	// Simple switcher
	// Add-----------------------
	float switcher_ = true;

	//
	// Device for fast proccessing
	//
	GPU gpu_;

	//
	// The global scene, with all objects
	//
	std::vector<Mesh<cl_float>> scene_;
};
