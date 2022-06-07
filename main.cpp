#include "Window.h"
#include <iostream>
using namespace std;

int main() 
{
	Window window;
	bool success = true;

	try {
		window.initialize(window);
	} 
	catch (int e) {
		cout << e << '\n';
		success = false;
	}

	if (success) 
	{
		//
		// Main executable field
		//
		while (window.getRun())
		{
			window.broadcast();
		}
	}

	return 0;
}