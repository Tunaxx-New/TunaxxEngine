#include <windows.h>
#pragma once
class Screen
{
public:
	Screen();
	BOOL undestroyed = true;
	COORD Window_size;
	struct point
	{
		int x = 0;
		int y = 0;
		COLORREF color = 0;
	};
	void Paint(HWND);
	void Start(COORD, RECT);
	void Resize(COORD, RECT);

	void SetInfo(std::vector<std::string>, std::vector<std::string>);
	void SetInfo(std::string, std::string, int);

	void pixel(point);
	void pixel(int, int, COLORREF);
	void pixel(int, int);
};