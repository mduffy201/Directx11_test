#include "GameApplication.h"

#include "../Window/Win32Window.h"
#include "../D3D11Renderer/D3D11Renderer.h"

//define screen resolution
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#include <fstream>
using namespace std;


CGameApplication::CGameApplication(void)
{
	m_pWindow = NULL;
	m_pRenderer = NULL;

	m_GameOptionDesc.gameName = L"GAME";
	m_GameOptionDesc.width = SCREEN_WIDTH;
	m_GameOptionDesc.height = SCREEN_HEIGHT;
	m_GameOptionDesc.fullscreen = false;

	m_ConfigFileName = L"game.cfg";
}


CGameApplication::~CGameApplication(void)
{
	if (m_pRenderer)
	{
		delete m_pRenderer;
		m_pRenderer = NULL;
	}
	if (m_pWindow)
	{
		delete m_pWindow;
		m_pWindow = NULL;
	}
}


bool CGameApplication::init()
{
	if (!parseConfigFile())
		return false;
	if (!initWindow())
		return false;
	if (!initGraphics())
		return false;
	if (!initInput())
		return false;
	if (!initGame())
		return false;
	
	return true;
}




bool CGameApplication::parseConfigFile()
{
	return true;
}


bool CGameApplication::initInput()
{
	return true;
}


bool CGameApplication::initPhysics()
{
	return true;
}

bool CGameApplication::initWindow()
{
	//creat a win32 window
	m_pWindow = new CWin32Window();
	m_pWindow->init(m_GameOptionDesc.gameName, m_GameOptionDesc.width, m_GameOptionDesc.height, m_GameOptionDesc.fullscreen);
	return true;
}

bool CGameApplication::initGraphics()
{
	m_pRenderer = new D3D11Renderer();

	if (!m_pRenderer->init(m_pWindow->getHandleToWindow(),
		m_GameOptionDesc.fullscreen))
		return false;

	return true;
}





bool CGameApplication::initGame()
{
	return true;
}


void CGameApplication::run()
{
	//while the window is not closed
	while (m_pWindow->running())
	{
		//check for all windows messages
		m_pWindow->checkForWindowMesseges();
		update();
		render();
	}
}

void CGameApplication::render()
{
	m_pRenderer->clear(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->render();
	m_pRenderer->present();
}

void CGameApplication::update()
{

}



