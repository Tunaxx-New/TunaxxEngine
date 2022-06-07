#include "Window.h"
#include "Constants.h"
#include "RasterEngineGPU.h"
#include "FileManager.h"
#include <iostream>


//
// Reference to class itself for onCreate, onUpdate, onDestroy in other scopes;
//
Window* window = 0;
FileManager<float>* fileManager = new FileManager<float>();

HWND _renderButton;
int renderType = 1;

//
// Engine that will draw in window
//
RasterEngineGPU* REGPU = new RasterEngineGPU();

//
// Standart broadcast handler in winapi to broadcast window messages
// @retrun callback message;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//
// Standart broadcast handler in winapi to broadcast window messages for Console
// @retrun callback message;
LRESULT CALLBACK WndProcConsole(HWND, UINT, WPARAM, LPARAM);

//
// Listener for enter in input window
// @return callback message key
LRESULT CALLBACK WndProcConsoleInput(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC          WndProcConsoleInputHandle = {};

//
// Listener for enter in output window
// @return callback message key
LRESULT CALLBACK WndProcConsoleOutput(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC          WndProcConsoleOutputHandle = {};

//
// Function for create timer process
// @params UINT - milliseconds
int timerProc(UINT ms);

//
// Games snake
//
Window::Snake snakeGame;


void Window::initialize(Window& wnd) throw(int)
{
	window = &wnd;
	//
	// Initializing Main window
	//
	WNDCLASSEX wc{};
	wc.cbClsExtra    = NULL;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.cbWndExtra    = NULL;
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance     = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.style         = NULL;
	wc.lpfnWndProc   = WndProc;


	if (!::RegisterClassEx(&wc))
		throw 0;


	hWindow_ = ::CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW, // ExStyle
		WINDOW_CLASS_NAME,      // Class Name
		WINDOW_NAME,            // Window Name
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,    // Style
		WINDOW_X,               // Position x
		WINDOW_Y,               // Position y
		WINDOW_WIDTH,           // Width
		WINDOW_HEIGHT,          // Height
		NULL,                   // HWND parent
		NULL,                   // hMenu
		NULL,                   // hInstance
		NULL                    // lpParam
	);

	if (!hWindow_)
		throw 1;

	if (!window)
		window = this;

	::ShowWindow(hWindow_, SW_SHOW);
	::UpdateWindow(hWindow_);

	//
	// Initializing console window
	//
	WNDCLASSEX wcConsole{};
	wcConsole.cbClsExtra    = NULL;
	wcConsole.cbSize        = sizeof(WNDCLASSEX);
	wcConsole.cbWndExtra    = NULL;
	wcConsole.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wcConsole.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcConsole.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wcConsole.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	wcConsole.hInstance     = NULL;
	wcConsole.lpszClassName = CONSOLE_CLASS_NAME;
	wcConsole.style         = NULL;
	wcConsole.lpfnWndProc   = WndProcConsole;

	if (!::RegisterClassEx(&wcConsole))
		throw 0;

	hConsole_ = ::CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,                   // ExStyle
		CONSOLE_CLASS_NAME,                       // Class Name
		CONSOLE_NAME,                             // Window Name
		WS_SYSMENU,                               // Style
		WINDOW_WIDTH - CONSOLE_WIDTH - MARGIN_LARGE + CONSOLE_X, //Align TO RIGHT of Window
		CONSOLE_Y + MARGIN_SMALL,                                //Align to TOP of Window
		CONSOLE_WIDTH + 3 * GetSystemMetrics(SM_CXSIZEFRAME),                                  // Width
		CONSOLE_HEIGHT + GetSystemMetrics(SM_CYCAPTION) + 5 * GetSystemMetrics(SM_CYSIZEFRAME),// Height
		NULL,                                     // HWND parent
		NULL,                                     // hMenu
		NULL,                                     // hInstance
		NULL                                      // lpParam
	);


	if (!hConsole_)
		throw 1;


	SetParent(hConsole_, hWindow_);
	::ShowWindow(hConsole_, SW_SHOW);
	::UpdateWindow(hConsole_);

	size_.X = WINDOW_WIDTH;
	size_.Y = WINDOW_HEIGHT;
	initializeBitmap();
	REGPU->initialize();
	memset(bitmapBack_, 0, buffer_ * sizeof(COLORREF));
}



void Window::initializeBitmap(std::string filePath)
{
	free(bitmapBack_);
	buffer_ = size_.X * size_.Y;
	bitmapBack_ = (COLORREF*)calloc(buffer_, sizeof(COLORREF));
	bitmapFront_ = CreateBitmap(size_.X, size_.Y, 1, 8 * 4, (void*)bitmapBack_);

	write(" = Main bitmap size is: " + std::to_string(size_.X) + 'x' + std::to_string(size_.Y) + " buffer: " + std::to_string(buffer_) + NEW_LINE);

	try {
		refillBitmap(filePath);
	}
	catch (int err) {
		write("Error in reading file: " + filePath + NEW_LINE);
	}

	//Positioning console
	SetWindowPos(hConsole_, NULL,
		size_.X - CONSOLE_WIDTH - MARGIN_LARGE + CONSOLE_X,										//Align TO RIGHT of Window
		CONSOLE_Y + MARGIN_SMALL,																//Align to TOP of Window
		CONSOLE_WIDTH + 3 * GetSystemMetrics(SM_CXSIZEFRAME),                                   // Width
		CONSOLE_HEIGHT + GetSystemMetrics(SM_CYCAPTION) + 5 * GetSystemMetrics(SM_CYSIZEFRAME), // Height
		0);

}



void Window::refillBitmap(std::string& filePath)
{
	// Random titles
	/*
	for (int i = 0; i < buffer_; i++)
	{
		int l = randRange(2) * 255;
		bitmapBack_[i] = RGB(l, l, l);
	}
	*/
	// From .lif titles
	std::string lifSource;
	std::pair<int, int> offset = { 0, 0 };
	if (filePath == "")
		fileManager->parseRLE(lifSource, FILE_OBJ_LIF, offset);
	else
		fileManager->parseRLE(lifSource, filePath, offset);
	int index = -1;
	for (int i = offset.second; i < size_.Y; i++)
		for (int j = offset.first; j < size_.X; j++)
		{
			index++;
			if (index >= lifSource.length())
				break;
			if (lifSource[index] == 'b')
				bitmapBack_[i * size_.X + j] = RGB(0, 0, 0);
			else if (lifSource[index] == 'o')
				bitmapBack_[i * size_.X + j] = RGB(255, 255, 255);
			else
				break;
		}
}



int Window::getBitmapSize()
{
	return buffer_;
}



void Window::broadcast()
{
	MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	window->onUpdate();

	Sleep(0);
}



void Window::release()
{
	REGPU->release();
	if (hConsole_)
		::DestroyWindow(hConsole_);
	if (hWindow_)
		::DestroyWindow(hWindow_);

	//Deleting memory pointers
	snakeGame.destroy();
	delete window;
	delete bitmapBack_;
}



void Window::onCreate()
{

}



void Window::onConsoleCreate(const HWND& hConsole)
{
	CreateWindow(
		TEXT("BUTTON"),
		L"LIF",
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		MARGIN_SMALL,
		MARGIN_SMALL,
		MARGIN_SMALL * 9,
		MARGIN_SMALL * 3,
		hConsole,
		(HMENU) OPEN_FILE_BUTTON,
		NULL,
		NULL
	);

	_renderButton = CreateWindow(
		TEXT("BUTTON"),
		L"1",
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		10 * MARGIN_SMALL,
		MARGIN_SMALL,
		MARGIN_SMALL * 3,
		MARGIN_SMALL * 3,
		hConsole,
		(HMENU) CHANGE_RENDER_BUTTON,
		NULL,
		NULL
	);

	hConsoleTextBoxIn_ = CreateWindow (
		TEXT("EDIT"),
		0,
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		MARGIN_SMALL,
		CONSOLE_HEIGHT - CONSOLE_TEXTBOX_HEIGHT - MARGIN_SMALL,
		CONSOLE_WIDTH - 3*MARGIN_SMALL,
		CONSOLE_TEXTBOX_HEIGHT,
		hConsole,
		NULL,
		NULL,
		NULL
	);
	WndProcConsoleInputHandle = (WNDPROC)SetWindowLongPtr(hConsoleTextBoxIn_, GWLP_WNDPROC, (LONG_PTR)WndProcConsoleInput);

	hConsoleTextBoxOut_ = CreateWindow(
		TEXT("EDIT"),
		0,
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
		MARGIN_SMALL,
		5 * MARGIN_SMALL,
		CONSOLE_WIDTH - 3 * MARGIN_SMALL,
		CONSOLE_HEIGHT - 7 * MARGIN_SMALL - CONSOLE_TEXTBOX_HEIGHT,
		hConsole,
		NULL,
		NULL,
		NULL
	);
	WndProcConsoleOutputHandle = (WNDPROC)SetWindowLongPtr(hConsoleTextBoxOut_, GWLP_WNDPROC, (LONG_PTR)WndProcConsoleOutput);
}



void Window::onUpdate()
{
	
}



void Window::onPaint()
{
	
	PAINTSTRUCT ps;
	HDC         hdc;
	HDC         src;
	BITMAPINFO  bitmapInfo;

	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
	bitmapInfo.bmiHeader.biWidth = size_.X;
	bitmapInfo.bmiHeader.biHeight = -size_.Y;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage = 0;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biClrUsed = 0;
	bitmapInfo.bmiHeader.biClrImportant = 0;

	hdc = BeginPaint(hWindow_, &ps);
	src = CreateCompatibleDC(hdc);

	SetDIBits(
		hdc,
		bitmapFront_,
		0,
		size_.Y,
		bitmapBack_,
		&bitmapInfo,
		DIB_RGB_COLORS
	);

	SelectObject(src, bitmapFront_);
	BitBlt(
		hdc,
		0,
		0,
		size_.X,
		size_.Y,
		src,
		0,
		0,
		SRCCOPY
	);
	DeleteDC(src);

	InvalidateRect(hWindow_, NULL, TRUE);

	EndPaint(hWindow_, &ps);
}



void Window::onRender()
{
	REGPU->draw(&bitmapBack_, buffer_, size_, renderType);
}



void Window::onDestroy()
{
	snakeGame.release();
	REGPU->release();
	run_ = false;
}



void Window::setFullscreen()
{
	if (fullscreen_) {
		SetWindowLong(hWindow_, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);
		SetWindowLong(hWindow_, GWL_EXSTYLE, WS_EX_OVERLAPPEDWINDOW);
		ShowWindow(hWindow_, SW_SHOWMAXIMIZED);
	}
	else {
		SetWindowLong(hWindow_, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN);
		SetWindowLong(hWindow_, GWL_EXSTYLE, WS_EX_TOPMOST);
		ShowWindow(hWindow_, SW_SHOWMAXIMIZED);
	}
	fullscreen_ = !fullscreen_;
	//setWindowSize(LOWORD(lparam), HIWORD(lparam));
	initializeBitmap();
}



void Window::showHideConsole()
{
	if (IsWindowVisible(hConsole_)) {
		ShowWindow(hConsole_, SW_HIDE);
		if (snakeGame.getIsRun())
			closeSnake();
	}
	else {
		ShowWindow(hConsole_, SW_SHOW);
	}
	SetWindowTextA(hConsoleTextBoxIn_, 0);
	UpdateWindow(hConsole_);
}



void Window::blitConsoleOut()
{
	//Reading text from input window box
	int size = GetWindowTextLength(hConsoleTextBoxIn_) + 1;
	LPSTR src = (PSTR)VirtualAlloc((LPVOID)NULL, (DWORD)size, MEM_COMMIT, PAGE_READWRITE);
	if (src != 0)
		GetWindowTextA(hConsoleTextBoxIn_, src, size);
	SetWindowTextA(hConsoleTextBoxIn_, 0);
	UpdateWindow(hConsoleTextBoxIn_);
	
	listenConsoleCommand(src);

	//Convert and blit text into output window box
	std::wstring stemp = std::wstring(consoleOutString_.begin(), consoleOutString_.end());
	LPCWSTR sw = stemp.c_str();
	SetWindowText(hConsoleTextBoxOut_, sw);
	UpdateWindow(hConsoleTextBoxOut_);
}



void Window::blitConsoleOutGame()
{
	std::wstring stemp = std::wstring(consoleOutString_.begin(), consoleOutString_.end());
	LPCWSTR sw = stemp.c_str();
	SetWindowText(hConsoleTextBoxOut_, sw);
	UpdateWindow(hConsoleTextBoxOut_);
}



void Window::write(std::string msg)
{
	consoleOutString_ += msg;
	blitConsoleOut();
}



void Window::listenConsoleCommand(LPSTR src)
{
	char* bufferToken;
	char* str;
	int   length = strlen(src);
	bool  clearConsole = false;

	pastCommand_ = src;

	if (length == 0)
		return;

	str = (char*)malloc(length * sizeof(char));
	strcpy_s(str, length + 1, src);

	if (strcmp(src, "/test") == 0) {
		consoleOutString_ += src;
		consoleOutString_ += NEW_LINE;
		consoleOutString_ += "Hello, world!";
	}
	else if (strcmp(src, "/clear") == 0) {
		clear();
	}
	else if (strcmp(src, "/info gpu") == 0) {
		consoleOutString_ += src;
		consoleOutString_ += NEW_LINE;
		consoleOutString_ += REGPU->getGPUInfo();
	}
	else if (strcmp(src, "/stop snake") == 0) {
		closeSnake(src);
	}
	else if (strcmp(strtok_s(str, " ", &bufferToken), "/snake") == 0) {
		int w = 0, h = 0, s = 0;
		int iterator = 6;
		bool error = true;
		if (src[iterator] != ' ') {
			consoleOutString_ += "error: Wrong form of command: ";
			consoleOutString_ += "Please, use '/snake width height speed'";
			iterator = length;
			error = false;
		}
		else {
			iterator++;
		}
		while (iterator < length)
		{
			//Get Width
			std::string arg = "";
			while (src[iterator] != ' ' && iterator < length) {
				arg += src[iterator];
				iterator++;
			}
			if (isNumber(arg)) {
				w = stoi(arg);
			}
			else {
				consoleOutString_ += "error: Wrong width argument: ";
				consoleOutString_ += "Please, use '/snake WIDTH height speed'";
				error = false;
			}
			arg = "";
			iterator++;

			//Get Height
			if (error) {
				while (src[iterator] != ' ' && iterator < length) {
					arg += src[iterator];
					iterator++;
				}
				if (isNumber(arg)) {
					h = stoi(arg);
				}
				else {
					consoleOutString_ += "error: Wrong height argument: ";
					consoleOutString_ += "Please, use '/snake width HEIGHT speed'";
					error = false;
				}
				arg = "";
				iterator++;
			}

			//Get Speed
			if (error) {
				while (iterator < length) {
					arg += src[iterator];
					iterator++;
				}
				if (isNumber(arg)) {
					s = stoi(arg);
				}
				else {
					consoleOutString_ += "error: Wrong speed argument: ";
					consoleOutString_ += "Please, use '/snake width height SPEED'";
					error = false;
				}
				arg = "";
			}
		}
		if (error) {
			if (w < 4) {
				consoleOutString_ += "error: Width size are too small: ";
				consoleOutString_ += "Please, use more than 3";
			}
			else if (h < 4) {
				consoleOutString_ += "error: Height size are too small: ";
				consoleOutString_ += "Please, use more than 3";
			}
			else if (s < 10 || s > 3000) {
				consoleOutString_ += "error: 10ms < Speed < 3000ms: ";
				consoleOutString_ += "Please, use another speed value";
			}
			else {
				consoleOutString_ += src;
				clearConsole = false;
				snakeGame.release();
				snakeGame.setSpeed(s);
				snakeGame.initialize(w, h);
			}
		}
	}
	else if (src[0] == '/') {
		consoleOutString_ += "error: Unknown command: ";
		consoleOutString_ += src;
	}
	else {
		consoleOutString_ += " - ";
		consoleOutString_ += src;
	}
	consoleOutString_ += NEW_LINE;

	if (clearConsole)
		clear();
}



void Window::clear()
{
	consoleOutString_ = "";
}



void Window::closeSnake()
{
	snakeGame.release();
	consoleOutString_ += "Your score is ";
	consoleOutString_ += std::to_string(snakeGame.getScore());
}



void Window::closeSnake(LPSTR src)
{
	snakeGame.release();
	consoleOutString_ += src;
	consoleOutString_ += "Your score is ";
	consoleOutString_ += std::to_string(snakeGame.getScore());
}



bool Window::getRun()
{
	return run_;
}



const HWND& Window::getHWindow()
{
	return hWindow_;
}



const HWND& Window::getHConsole()
{
	return hConsole_;
}



const HWND& Window::getHConsoleTextBoxIn()
{
	return hConsoleTextBoxIn_;
}



const HWND& Window::getHConsoleTextBoxOut()
{
	return hConsoleTextBoxOut_;
}



LPSTR Window::getPastCommand()
{
	return pastCommand_;
}



void Window::setConsoleOutString(std::string str)
{
	consoleOutString_ = str;
}



void Window::setWindowSize(int w, int h)
{
	size_.X = w;
	size_.Y = h;
}



LRESULT CALLBACK WndProc(HWND hWindow, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
		window->onCreate();
		break;

	case WM_COMMAND:
		break;

	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_OEM_3:
			window->showHideConsole();
			break;

		case VK_F11:
			window->setFullscreen();
			break;

		case VK_F5:
			REGPU->gswitch();
			break;

		case VK_F9:
			window->initializeBitmap();
			break;
		}
		break;

	case WM_ERASEBKGND:
		break;

	case WM_PAINT:
		window->onRender();
		window->onPaint();
		break;

	case WM_SIZE:
		window->setWindowSize(LOWORD(lparam), HIWORD(lparam));
		if (wparam > 0) {
			window->initializeBitmap();
		}
		break;

	case WM_EXITSIZEMOVE:
	case WM_ENTERSIZEMOVE:
		REGPU->gswitch();
		window->initializeBitmap();
		break;

	case WM_DESTROY:
		window->onDestroy();
		break;

	default:
		if (hWindow)
			return ::DefWindowProc(hWindow, msg, wparam, lparam);
	}

	return NULL;
}



LRESULT CALLBACK WndProcConsole(HWND hConsole, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (wparam)
		{
		case OPEN_FILE_BUTTON:
		{
			REGPU->gswitch();
			OPENFILENAME open;
			wchar_t filePath[100];

			ZeroMemory(&open, sizeof(OPENFILENAME));

			open.lStructSize = sizeof(OPENFILENAME);
			open.lpstrFile = filePath;
			open.lpstrFile[0] = '\0';
			open.nMaxFile = 100;
			open.lpstrFilter = L"LIF\0*.LIF\0";
			open.nFilterIndex = 1;

			GetOpenFileName(&open);
			std::wstring ws(filePath);
			std::string path(ws.begin(), ws.end());
			window->write("Openned lif file " + path + NEW_LINE);

			window->initializeBitmap(path);
			REGPU->gswitch();
			break;
		}

		case CHANGE_RENDER_BUTTON:
		{
			if (renderType == 1)
				renderType = 0;
			else
				renderType = 1;
			std::string renderString = std::to_string(renderType);
			std::wstring stemp = std::wstring(renderString.begin(), renderString.end());
			LPCWSTR sw = stemp.c_str();

			_renderButton = NULL;
			_renderButton = CreateWindow(
				TEXT("BUTTON"),
				sw,
				WS_CHILD | WS_VISIBLE | WS_BORDER,
				10 * MARGIN_SMALL,
				MARGIN_SMALL,
				MARGIN_SMALL * 3,
				MARGIN_SMALL * 3,
				hConsole,
				(HMENU)CHANGE_RENDER_BUTTON,
				NULL,
				NULL
			);
			break;
		}
		}
		break;
		

	case WM_CREATE:
		window->onConsoleCreate(hConsole);
		break;

	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_OEM_3:
			window->showHideConsole();
			break;

		case VK_RETURN:
			window->blitConsoleOut();
			break;

			//For games--------------------
		case VK_NUMPAD8:
			snakeGame.setDirection(0);
			break;

		case VK_NUMPAD6:
			snakeGame.setDirection(1);
			break;

		case VK_NUMPAD2:
			snakeGame.setDirection(2);
			break;

		case VK_NUMPAD4:
			snakeGame.setDirection(3);
			break;
			//-----------------------------
		}
		break;

	case WM_CLOSE:
		ShowWindow(hConsole, SW_HIDE);
		return 0;

	case WM_DESTROY:
		if (snakeGame.getIsRun())
			KillTimer(hConsole, TIMER_SNAKE);
		break;

	default:
		if (hConsole)
			return ::DefWindowProc(hConsole, msg, wparam, lparam);
	}

	return NULL;
}



LRESULT CALLBACK WndProcConsoleInput(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_RETURN:
			window->blitConsoleOut();
			break;

		case VK_OEM_3:
			window->showHideConsole();
			break;

		case VK_UP:
			SetWindowTextA(hwnd, window->getPastCommand());
			UpdateWindow(hwnd);
			break;

		//For games--------------------
		case VK_NUMPAD8:
			snakeGame.setDirection(0);
			break;

		case VK_NUMPAD6:
			snakeGame.setDirection(1);
			break;

		case VK_NUMPAD2:
			snakeGame.setDirection(2);
			break;

		case VK_NUMPAD4:
			snakeGame.setDirection(3);
			break;
		//-----------------------------
		}

	case WM_HOTKEY:
		if (GetKeyState('A') & 0x8000 && GetKeyState(VK_CONTROL) & 0x8000)
			SendMessage(hwnd, EM_SETSEL, 0, -1);
		break;
	default:
		return CallWindowProc(WndProcConsoleInputHandle, hwnd, msg, wparam, lparam);
	}
	return 0;
}



LRESULT CALLBACK WndProcConsoleOutput(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_RETURN:
			window->blitConsoleOut();
			break;

			//For games--------------------
		case VK_NUMPAD8:
			snakeGame.setDirection(0);
			break;

		case VK_NUMPAD6:
			snakeGame.setDirection(1);
			break;

		case VK_NUMPAD2:
			snakeGame.setDirection(2);
			break;

		case VK_NUMPAD4:
			snakeGame.setDirection(3);
			break;
			//-----------------------------
		}
		break;

	default:
		return CallWindowProc(WndProcConsoleOutputHandle, hwnd, msg, wparam, lparam);
	}
	return 0;
}



int timerProc(UINT ms)
{
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -10000 * (ULONGLONG)ms;
	hTimer = CreateWaitableTimer(NULL, TRUE, L"Timer");

	if (NULL == hTimer)
		return -1;

	MSG msg;
	while (snakeGame.getIsRun()) {
		::GetMessage(&msg, 0, 0, 0);
		::DispatchMessage(&msg);
		if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
			printf("SetWaitableTimer failed (%lu)\n", GetLastError());
			return 2;
		}

		// Wait for the timer.
		if (!(WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)) {
			::GetMessage(&msg, 0, 0, 0);
			::DispatchMessage(&msg);
			window->setConsoleOutString(snakeGame.blitMap());
			window->blitConsoleOutGame();
			snakeGame.move();
		}
	}

	window->listenConsoleCommand((char*)"Game over");
	window->listenConsoleCommand((char*)"/stop snake");
	window->blitConsoleOut();

	return 0;
}



int Window::SysGame::getSpeed()
{
	return speed_;
}



void Window::SysGame::setSpeed(int s)
{
	speed_ = s;
}



bool Window::SysGame::getIsRun()
{
	return isRun_;
}



void Window::Snake::initialize(int w, int h)
{
	width_  = w * ratio_ + 2;
	height_ = h;
	isRun_  = true;
	body_.resize(w * h);
	bodySize_ = 3;
	map_ = std::string(width_ * height_, ' ');
	for (int i = 0; i < height_; i++) {
		for (int j = 0; j < width_; j += ratio_) {
			map_[i * width_ + j] = gradient_[0];
		}
	}
	for (int i = 0; i < h; i++) {
		map_[i * width_ + w * ratio_] = '\r';
		map_[i * width_ + w * ratio_ + 1] = '\n';
	}

	COORD s1 = { ratio_ * (w / 2), h / 2 };
	COORD s2 = { s1.X, s1.Y + 1 };
	COORD s3 = { s1.X, s1.Y + 2 };

	body_[0] = s1;
	body_[1] = s2;
	body_[2] = s3;

	timerProc(speed_);
	replaceApple();
}



std::string Window::Snake::blitMap()
{
	int color = getGradientSize(gradient_);
	int ratio = bodySize_ / color;
	int iterator = 0;
	color--;
	for (int i = 0; i < bodySize_ - 1; i++) {
		map_[width_ * body_[i].Y + body_[i].X ] = gradient_[color];
		iterator++;
		if (iterator > ratio) {
			iterator = 0;
			color--;
		}
		if (color < 0)
			color = 0;
	}
	map_[width_ * body_[bodySize_ - 1].Y + body_[bodySize_ - 1].X] = gradient_[0];
	map_[width_ * apple_.Y + apple_.X] = '*';
	return map_;
}



void Window::Snake::move()
{
	//Checking crash
	for (int i = 1; i < bodySize_; i++) {
		if (body_[0].X == body_[i].X &&
			body_[0].Y == body_[i].Y) {
			isRun_ = false;
			return;
		}
	}

	//Moving body
	for (int i = bodySize_ - 1; i > 0; i--) {
		body_[i] = body_[i - 1];
	}

	//Checking apple
	if (body_[0].X == apple_.X &&
		body_[0].Y == apple_.Y) {
		body_[bodySize_] = body_[bodySize_ - 1];
		if (bodySize_ < body_.size() - 1) {
			bodySize_++;
		}
		replaceApple();
		score_++;
	}

	switch (direction_)
	{
	case 0:
		body_[0].Y--;
		break;

	case 1:
		body_[0].X += ratio_;
		break;

	case 2:
		body_[0].Y++;
		break;

	case 3:
		body_[0].X -= ratio_;
		break;
	}

	if (body_[0].X >= width_ - 2)
		body_[0].X = 0;
	if (body_[0].X < 0)
		body_[0].X = width_ - 2 - ratio_ + (width_ - 2) % ratio_;
	if (body_[0].Y >= height_)
		body_[0].Y = 0;
	if (body_[0].Y < 0)
		body_[0].Y = height_ - 1;
}



void Window::Snake::release()
{
	width_  = 0;
	height_ = 0;
	isRun_  = false;
	body_.resize(0);
	bodySize_ = 0;
}



void Window::Snake::setDirection(char d)
{
	bool dir1 = direction_ % 2;
	bool dir2 = d % 2;

	if (dir1 ^ dir2)
		direction_ = d;
}



void Window::Snake::replaceApple()
{
	apple_.X = randRange((width_ - 1) / ratio_) * ratio_;
	apple_.Y = randRange(height_);
}



int Window::Snake::getScore()
{
	return score_;
}



void Window::Snake::destroy()
{
	delete gradient_;
}



bool isNumber(const std::string& str)
{
	if (str.length() == 0)
		return false;
	for (char const& c : str) {
		if (std::isdigit(c) == 0)
			return false;
	}
	return true;
}



int getGradientSize(const char* g)
{
	return sizeof(g) / sizeof(g[0]);
}



int randRange(int a)
{
	if (a == 0)
		return 0;
	return (rand() * 10000 + rand()) % a;
}