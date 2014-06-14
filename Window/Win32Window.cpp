#include "Win32Window.h"

//Global pointer (trick to make the win32 window object orientated)
CWin32Window * g_pWindow = NULL;


//callback function for windows messages (another trick to make window object oriented)
LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	//call the windowProc function inside the Win32Window class
	return g_pWindow->WindowProc(hWnd, uMessage, wParam, lParam);
}


CWin32Window::CWin32Window(void)
{
	//(part of trick)
	g_pWindow = this;
}

CWin32Window::~CWin32Window(void)
{
	g_pWindow = NULL;
}

bool CWin32Window::init(const wstring &title, int width, int height, bool fullscreen)
{
	m_bIsFullScreen = fullscreen;
	HINSTANCE currentInstance = GetModuleHandle(NULL);

	//struct holds info for window class
	WNDCLASSEX wndClass;

	//clear window class for use
	ZeroMemory(&wndClass, sizeof(WNDCLASSEX));

	wndClass.cbSize = sizeof(WNDCLASSEX);							//size of wnd class structure
	wndClass.style = CS_HREDRAW | CS_VREDRAW;						//window style
	wndClass.lpfnWndProc = StaticWindowProc;						//function to handle windows messages
	wndClass.cbClsExtra = 0;										//extra class info
	wndClass.cbWndExtra = 0;										//extra window info
	wndClass.hInstance = currentInstance;							//application instance
	wndClass.hIcon = LoadIcon(currentInstance, IDI_APPLICATION);	//window icon
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);					//window cursor
	wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;					//background colour (Remove for full screen?)
	wndClass.lpszMenuName = NULL;									//menu name
	wndClass.lpszClassName = L"WindowClass1";						//window class name
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);				//small icon


	//Register window class (all subsequent calls to CreateWindowEx will use above class
	if (!RegisterClassEx(&wndClass))
		return false;

	//Calculate actual size of window (excludes borders/menus)
	RECT wr = { 0, 0, width, height }; //set size but not position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); //adjust the size

	//window style (change if going fullscreen
	DWORD style = WS_OVERLAPPEDWINDOW;

	//change style if in fullsceen
	if (fullscreen)
	{
		style = WS_POPUP;		//change style
		ShowCursor(FALSE);		//hide cursor
	}

	//create the window and use result as handle
	m_hWHD = CreateWindowEx(
		NULL,									//extra styles
		L"WindowClass1",						//Name of window class
		L"Our first Direct3D program",			//title of window
		style,									//window style
		300,									//x position of window
		300,									//y position of window
		wr.right - wr.left,					//width of window
		wr.bottom - wr.top,					//height of window
		//width,
		//height,
		//NULL,									//no parent window
		GetDesktopWindow(),						//a handle to parent window
		NULL,									//handle to menu (not using menus)
		currentInstance,						//application instance handle
		NULL);									//window creation data (used with multiple windows)

	////if window handle is null, somthing went wrong
	if (!m_hWHD)
		return false;

	//display window on screen
	ShowWindow(m_hWHD, SW_SHOWDEFAULT);

	//update window, triggers a "pain"?
	UpdateWindow(m_hWHD);

	//If successful set running to true
	m_bIsRunning = true;

	return true;
}

bool CWin32Window::checkForWindowMesseges()
{
	//===================
	//enter MAIN LOOP
	//===================

	//struct holds windows event messages
	MSG msg;

	//check to see if any meesages waiting in queue (dont wait for message)
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0){

		//translate keystroke messages into correct format
		TranslateMessage(&msg);

		//send message to WindowProc function
		DispatchMessage(&msg);

		return true;
	}
	else
	{
		return false;
	}
}

//main message handler for the program (windows message pump)
//return message result (LRESULT)
LRESULT CALLBACK CWin32Window::WindowProc(
	HWND hWnd,								// The window that is recieving messages
	UINT uMessage,							//The message type
	WPARAM wParam,							//The wide parameter of the message
	LPARAM lParam)							//The low parameter of the message
{

	//sort through and find what code to run for the message given
	switch (uMessage)
	{
	case WM_DESTROY:
	{
		//set running to false
		m_bIsRunning = false;

		//Close application
		PostQuitMessage(0);
		break;
	}
	default:
	{
		//handle any messages the switch statement didnt
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
	}
	return 0;
}
