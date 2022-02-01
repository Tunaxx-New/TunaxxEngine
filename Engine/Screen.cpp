#include <iostream>															// for console debug
#include <windows.h>														// window lib
#include "../ExternData.h"
#include "DirectY.h"
#include "Engine.h"
#include <string>
#include <vector>
#include <atlstr.h>
#include <utility>

#define COUNT_DEBUG  2
#define DEBUG_RIGHT  40
#define DEBUG_HEIGHT 16
#define COLOR_WHITE  16777215

class Screen															    // Screeb class for painting and blitting image
{																			//
public:																		//public:
	Screen();																// Screen constructor decline
	~Screen();																// Screen destructor decline

	struct point
	{
		int x = 0;
		int y = 0;
		COLORREF color = 0;
	};
	int			width = 0;

	BOOL  undestroyed = true;
	COORD Window_size = COORD{0, 0};										// Size of window mypc(1980x1080)
																			//
	void Paint(HWND);														// Paint method decline
	void Start(COORD, RECT);
	void Resize(COORD, RECT);
	void SetInfo(std::vector<std::string>, std::vector<std::string>);
	void SetInfo(std::string, std::string, int);

	//Pixel overload
	void pixel(point);
	void pixel(int, int, COLORREF);
	void pixel(int, int);
private:																	//private:
	CHAR*	    info_text_debug[COUNT_DEBUG]  = {};
	CHAR*	    info_const_debug[COUNT_DEBUG] = {};
	size_t		info_size_debug[COUNT_DEBUG]  = {};
	COLORREF* bmp_world  = 0;											// array of pixels
	COLORREF* bmp_freeze = 0;											// Blit when the another arrays computing
	HBITMAP   TextMap	 = 0;

	PAINTSTRUCT ps		     = PAINTSTRUCT{};								// own paint struct
	HDC		    hdc			 = 0;											// own handle device context
	HDC		    tdc			 = 0;
																			//
																			//
	COLORREF*	ptr			 = 0;
	COORD	    screen_start = COORD{ 0, 0 };								// screen start (0, 30 cause caption)
	COORD		screen_end   = COORD{ 0, 0 };								// screen end

	int			screen_size  = 0;											// pixels count
	int			scenter      = 0;
	int			buffsize	 = 0;											// height in pixel coordinate computing

	COLORREF	info_text_brush = RGB(255, 255, 255);

	void info_text() {
		std::string frames_str = "";
		if (undestroyed) {
			frames_str = std::to_string(FPS);
			info_size_debug[0] = strlen(frames_str.c_str()) + 1;
			info_text_debug[0] = (char*)frames_str.c_str();
		}

		RECT debug_rect = { 0, DEBUG_HEIGHT, DEBUG_RIGHT, DEBUG_HEIGHT*2 };
		int debug_right = debug_rect.left + DEBUG_RIGHT;

		SetTextColor(tdc, info_text_brush);
		SetBkMode(tdc, TRANSPARENT);
		for (int i = 0; i < COUNT_DEBUG; i++) {
			debug_rect.top += DEBUG_HEIGHT;
			debug_rect.bottom += DEBUG_HEIGHT;
			TextOutA(tdc, debug_right, debug_rect.top, info_text_debug[i], info_size_debug[i]);
			TextOutA(tdc, debug_rect.left, debug_rect.top, info_const_debug[i], info_size_debug[i]);
		}
	}

	//BITMAP MANIPULATIONS
	//
	void locating(COORD screen_starts) {									// locate memory for arrays
		screen_size = Window_size.X * Window_size.Y;						// 1980x1080
		width = Window_size.X;												//
		screen_end = Window_size;
		screen_start = screen_starts;
		scenter = Window_size.X * (Window_size.Y / 2) + width / 2;
		buffsize = screen_size * sizeof(COLORREF);
		bmp_world = (COLORREF*)calloc(screen_size, sizeof(COLORREF));		// calloc in heap
		bmp_freeze = (COLORREF*)calloc(screen_size, sizeof(COLORREF));		// calloc in heap
	}																		//
	void delocating_bmp() {													// free all bmps
		free(bmp_world);
		free(bmp_freeze);
		free(info_text_debug);												// free world
		free(info_const_debug);
	}																		//
	void fill_bmp() {
		for (int i = 0; i < screen_size; i++) {
			bmp_world[i] = RGB(25, 25, 0);
			//bmp_freeze[i] = RGB(screen_size - i, screen_size* screen_size - i*i, screen_size* screen_size* screen_size - i*i*i);
			bmp_freeze[i] = RGB(0, 0, 0);
		}
	}
	void BtS_world() {														//https://stackoverflow.com/questions/26005744/how-to-display-pixels-on-screen-directly-from-a-raw-array-of-rgb-values-faster-t
		HDC src = CreateCompatibleDC(hdc);									//Comatible Device Context
		HBITMAP bmp = CreateBitmap(Window_size.X,							// width
			Window_size.Y,													// height
			1,																// Color Planes, unfortanutelly don't know what is it actually. Let it be 1
			8 * 4,															// Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
			(void*)bmp_world);												// our array
		SelectObject(src, bmp);												//selecting bmp world													
		BitBlt(																//Blitting to screen hdc
			hdc,															//	Screen HDC
			screen_start.X,													//  x0
			screen_start.Y,													//  y0
			screen_end.X,													//  x1
			screen_end.Y,													//  y1
			src,															//	source
			0,																//	dx
			0,																//  dy
			SRCPAINT														    //  COPY AND PASTE from source
		);																	//
		DeleteDC(src);														//  free our source
		DeleteObject(bmp);
	}
	void BtS_freeze() {													
		HDC src = CreateCompatibleDC(hdc);									//Comatible Device Context
		HBITMAP bmp = CreateBitmap(Window_size.X,							// width
			Window_size.Y,							// height
			1,										// Color Planes, unfortanutelly don't know what is it actually. Let it be 1
			8 * 4,									// Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
			(void*)bmp_freeze);						// our array
		SelectObject(src, bmp);												//selecting bmp world													
		BitBlt(																//Blitting to screen hdc
			hdc,															//	Screen HDC
			screen_start.X,													//  x0
			screen_start.Y,													//  y0
			screen_end.X,													//  x1
			screen_end.Y,													//  y1
			src,															//	source
			0,																//	dx
			0,																//  dy
			SRCPAINT														    //  COPY AND PASTE from source
		);																	//
		DeleteDC(src);														//  free our source
		DeleteObject(bmp);
	}
};																			// End of the class decline
																			//
Screen::Screen(void) {														// Constructor
	hdc = ps.hdc;
}																			//
Screen::~Screen(void) {														// Destructor
	delocating_bmp();														//free bmps
}
	
void Screen::Start(COORD screen_start, RECT window) {
	Window_size.X = window.right;
	Window_size.Y = window.bottom;
	locating(screen_start);
	for (int c = 0; c < COUNT_DEBUG; c++)
	{
		info_text_debug[c] = (CHAR*)calloc(128, sizeof(CHAR));
		info_const_debug[c] = (CHAR*)calloc(128, sizeof(CHAR));
	}
	info_const_debug[0] = (char*)"FPS:";
	fill_bmp();
}

void Screen::SetInfo(std::vector<std::string> args, std::vector<std::string> name_args) {
	if (undestroyed) {
		for (int i = 1; i <= args.size(); ++i)
		{
			strcpy(info_text_debug[i], args[i - 1].c_str());
			if (strcmp(args[i - 1].c_str(), name_args[i - 1].c_str()) < 0) {
				info_size_debug[i] = args[i - 1].length();
			} else info_size_debug[i] = name_args[i - 1].length();
			strcpy(info_text_debug[i], args[i - 1].c_str());
		}
	}
}
void Screen::SetInfo(std::string arg, std::string name_arg, int index) {
	if (undestroyed) {
		strcpy(info_text_debug[index], arg.c_str());
		if (strcmp(arg.c_str(), name_arg.c_str()) < 0) {
			info_size_debug[index] = arg.length();
		}
		else info_size_debug[index] = name_arg.length();
		strcpy(info_const_debug[index], name_arg.c_str());
	}
}

void Screen::Resize(COORD screen_start, RECT window) {
	Window_size.X = window.right;
	Window_size.Y = window.bottom;
	locating(screen_start);
}
void Screen::Paint(HWND hWnd) {												//Updating paint method
	hdc = BeginPaint(hWnd, &ps);											//own hdc start paint
	frames++;

	//BtS_freeze();
	if (bmp_freeze) {
		memset(bmp_freeze, 0, buffsize);
	}
	if (bmp_world) {
		memset(bmp_world, 0, buffsize);
	}

	Draw_Directly();
	DrawScreen();
	BtS_world();

	tdc = CreateCompatibleDC(hdc);
	TextMap = CreateCompatibleBitmap(hdc, Window_size.X, Window_size.Y);
	SelectObject(tdc, TextMap);
	info_text();
	BitBlt(hdc, 0, 0, Window_size.X, Window_size.Y, tdc, 0, 0, SRCCOPY);
	DeleteObject(TextMap);

	EndPaint(hWnd, &ps);													//end painting
}

//pixel overload
void Screen::pixel(point pixel) {
	int coordiante = width * pixel.y + pixel.x + scenter;
	if (bmp_world) {
		bmp_world[coordiante] = pixel.color;
	}
	if (bmp_freeze) {
		bmp_freeze[coordiante] = pixel.color;
	}
}
void Screen::pixel(int x, int y, COLORREF col) {
	int coordiante = width * y + x + scenter;
	if (bmp_world) {
		bmp_world[coordiante] = col;
	}
	if (bmp_freeze) {
		bmp_freeze[coordiante] = col;
	}
}
void Screen::pixel(int x, int y) {
	//Checking belong
	int linear = width * y + x;
	if (scenter <= linear || linear <= -scenter) return;

	int coordiante = linear + scenter;
	if (bmp_world) {
		try { bmp_world[coordiante] = COLOR_WHITE; }
		catch (char* e) {}
	}
	if (bmp_freeze) {
		try {
			bmp_freeze[coordiante] = COLOR_WHITE;
		}
		catch (char* e) {}
	}
}