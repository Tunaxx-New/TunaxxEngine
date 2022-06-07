#pragma once
#pragma warning( disable : 4290 )
#include <Windows.h>
#include <string>
#include <vector>

//
// Window - main application window
// Will blitting engine results bitmap and custom console
class Window 
{
public:
	//
	// Class for console mini-games
	//
	class SysGame
	{
	public:
		//
		// Method for getting speed
		// @return speed in milliseconds
		int getSpeed();

		//
		// Method for setting speed
		// 
		void setSpeed(int);

		//
		// @return isRun_
		//
		bool getIsRun();
	protected:

		//
		// Boolean is game running
		//
		bool isRun_ = false;

		//
		// Width of map
		//
		int width_ = 0;

		//
		// Height of map
		//
		int height_ = 0;

		//
		// Gradient with chararcters ASCII
		//
		const char gradient_[9] = { '.', ':', ';', '+', '=', 'x', 'X', '$', '&' };
		
		//
		// Speed for every milliseconds in timer
		//
		int speed_ = 1000;

		//
		// Ration of console symbols
		// 2 - means 1 : 2
		int ratio_ = 4;
	};

	//
	// Class snake
	// @game = /snake
	class Snake : public SysGame
	{
	public:
		//
		// Initializing game setting width, height, body, isRun
		//
		void initialize(int, int);

		//
		// Procedure for draw in consoleOutString game map with snake
		//
		std::string blitMap();

		//
		// Procedure for move snake body
		//
		void move();

		//
		// Release game setting width, height, body, isRun
		//
		void release();

		//
		// Procedure for move snake body
		//
		void setDirection(char);

		//
		// Procedure for move apple randomly
		//
		void replaceApple();

		//
		// @return score_
		//
		int getScore();

		//
		// destroy - delete[] all arrays
		//
		void destroy();
	private:
		//
		// Snake body
		//
		std::vector<COORD> body_{ 0 };

		//
		// Snake body part number
		//
		int bodySize_ = 0;

		//
		// Snake direction to move
		//
		char direction_ = 0;

		//
		// Map with \n s
		//
		std::string map_ = "";

		//
		// Score
		//
		int score_ = 0;

		//
		// Apple position
		//
		COORD apple_ = { 0, 0 };
	};

	// Initializing winapi window
	// @return is Window initialize properly;
	// @throws error of initialization
	//   0 - RegisterClassEx is null
	//   1 - hWnd is null
	void initialize(Window&) throw(int);

	//
	// Initializing bitmap or resizing
	//
	void initializeBitmap();

	//
	// get bitmap front && back size
	// @return BitmapFront and Back size
	int getBitmapSize();

	//
	// Broadcasting winapi messages and translate them
	// @retrun is Message translated and accepted properly;
	void broadcast();

	//
	// Release and delete winapi window
	// @return is Release executes properly
	void release();

	//
	// onCreate calling when window are showed in ONCREATE WM message
	//
	void onCreate();

	//
	// onCreate for Console same as Window
	//
	void onConsoleCreate(const HWND&);

	//
	// onUpdate calling in broadcast method
	//
	void onUpdate();

	//
	// onPaint calling in message method WM_PAINT
	//
	void onPaint();
	//
	// onPaint calling in message method WM_PAINT
	//
	void onRender();

	//
	// onDestroy calling when window are showed in ONDESTROY WM message
	//
	void onDestroy();

	//
	// Procedure to fullscreen window
	//
	void setFullscreen();

	//
	// Procedure to hide or show console window
	//
	void showHideConsole();

	//
	// Procedure to blit console text
	// and read commands from input window
	void blitConsoleOut();

	//
	// Procedure to blit console text
	//
	void blitConsoleOutGame();

	//
	// Procedure to write in console
	//
	void write(std::string);

	//
	// Procedure to manage the input commands
	//
	void listenConsoleCommand(LPSTR);

	//
	// Procedure to manage the input commands
	//
	void clear();

	//
	// Procedure to close snake and output score
	//
	void closeSnake();

	//
	// Procedure to close snake and output score
	//
	void closeSnake(LPSTR);

	//
	// Is window are running. Uses in main while();
	// @return bool run;
	bool getRun();

	//
	// @return Handle of Window
	//
	const HWND& getHWindow();

	//
	// @return Handle of Console window
	//
	const HWND& getHConsole();

	//
	// @return Handle of Console window input
	//
	const HWND& getHConsoleTextBoxIn();

	//
	// @return Handle of Console window output
	//
	const HWND& getHConsoleTextBoxOut();

	//
	// @return Handle of Console window output
	//
	LPSTR getPastCommand();

	//
	// Setting the console output text
	//
	void setConsoleOutString(std::string);

	//
	// Setting size of window
	//
	void setWindowSize(int, int);
private:
	//
	// bitmap front
	//
	HBITMAP bitmapFront_ = 0;

	//
	// bitmap back
	//
	COLORREF* bitmapBack_ = 0;

	//
	// bitmap sizes
	//
	int buffer_ = 0;

	//
	// Window sizes
	//
	COORD size_ = { 0, 0 };

	//
	// Is application active in main's while();
	//
	bool run_ = true;

	//
	// Is application active in main's while();
	//
	bool fullscreen_ = false;

	//
	// Handle of Main window
	//
	HWND hWindow_ = 0;

	//
	// Handle of Console window
	//
	HWND hConsole_ = 0;

	//
	// Handle of Console Text Box, where inputs console commands
	//
	HWND hConsoleTextBoxIn_ = 0;

	//
	// Handle of Console Text Box, where outputs console commands
	//
	HWND hConsoleTextBoxOut_ = 0;

	//
	// Handle of Console Text Box, where outputs console commands
	//
	std::string consoleOutString_ = "";

	//
	// Past command saved in buffer
	//
	LPSTR pastCommand_ = 0;
};

//
// Helping function is string contains only digits
// @return is it true or false
bool isNumber(const std::string&);

//
// Helping function to get array size
// @return array size
int getGradientSize(const char*);

//
// Helping function to generate random in range
// @return random in range [0 - r)
int randRange(int);