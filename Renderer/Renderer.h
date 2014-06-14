#pragma once

//Interface class
class IRenderer
{
public:
	//always has to have an empty deconstrucor
	virtual ~IRenderer(){};
	//pure virtual functions have to be implemented
	virtual bool init(void *pWindowHandle, bool fullscreen) = 0;
	virtual void clear(float r, float g, float b, float a) = 0;
	virtual void present() = 0;
	virtual void render() = 0;
};