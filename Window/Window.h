#pragma once

#include<string>

using namespace std;

//Interface for a window
//Classes will impliment this 
//Will allow the creation of other windows (Windows, Linux, Mac, PS3)
class IWindow
{
public:
	//always have to implement an empty virtual destructor when we define an c++ interface
	virtual ~IWindow(){};

	//pure virtual function (must implement in all base classes
	//will initalise the window
	virtual bool init(const wstring &title, int width, int height, bool fullscreen) = 0;

	//is running?
	virtual bool running() = 0;

	//is full screen?
	virtual bool isFullScreen() = 0;

	//check for windows messages
	virtual bool checkForWindowMesseges() = 0;

	//get a handle to the window (some APIs require a handle(pointer) to a window
	virtual void* getHandleToWindow() = 0;

};