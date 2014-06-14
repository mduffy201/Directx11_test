#pragma once

#include<string>

using namespace std;

//Forward declarations
class IWindow;			//window interface
class IRenderer;		//renderer interface


//Structure for holding GameOptions (this will be loaded from config files)
struct GameOptionsDesc
{
	wstring gameName;
	int width;
	int height;
	bool fullscreen;
};

//Game Application Class
class CGameApplication
{
public:
	CGameApplication(void);				//Constructor
	virtual ~CGameApplication(void);	//Deconstructor
	virtual bool init();				//Initalise all subsystems
	void run();							//Called to put the game into the loop
	virtual void render();				//Called to draw a single frame of the game
	virtual void update();				//Called to update the game

private:
	bool parseConfigFile();			//Called to parse the config file
	bool initInput();				//Initialises the input subsystem
	bool initPhysics();				//Initialises the physics subsystem
	bool initGraphics();			//Initalises the graphics subsystem
	bool initWindow();				//Initalise the window
	bool initGame();				//Initalise the game


private:
	//pointer types
	IWindow * m_pWindow;
	IRenderer * m_pRenderer;

	GameOptionsDesc m_GameOptionDesc;
	wstring m_ConfigFileName;

};