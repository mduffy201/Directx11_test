#pragma once;

//Win32 header file
#include <Windows.h>

//string header from c++ std library
#include <string>

//window interface
#include "Window.h"

using namespace std;

//Implimentation of a win32 window, inherets from the window interface
class CWin32Window :public IWindow
{
public:
	//Constructor/Destructor
	CWin32Window(void);
	~CWin32Window(void);


	//Initalise window
	bool init(const wstring &title, int width, int height, bool fullscreen);

	bool running()
	{
		return m_bIsRunning;
	};

	bool isFullScreen()
	{
		return m_bIsFullScreen;
	};

	//Window callback function, this called when a windows messahe is available
	LRESULT CALLBACK WindowProc(HWND hWnd,
		UINT uMessage,
		WPARAM wParam,
		LPARAM lParam);

	bool checkForWindowMesseges();

	void* getHandleToWindow()
	{
		return m_hWHD;
	};

private:
	bool m_bIsRunning;
	bool m_bIsFullScreen;
	HWND m_hWHD;


};