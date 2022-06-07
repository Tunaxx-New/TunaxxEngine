#pragma once
#include <Windows.h>
class Engine
{
public:
	//
	// Method for initializion the engine
	//
	virtual void initialize() = 0;

	//
	// Method for drawing in bitmapBack_ from main window
	// @params COLORREF** reference to array of pixels;
	virtual void draw(COLORREF**, int, COORD, int) = 0;

	//
	// Method for release the engine
	//
	virtual void release() = 0;
private:

};
