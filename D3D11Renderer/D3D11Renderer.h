#pragma once;

//include renderer interface
#include "../Renderer/Renderer.h"
#include <Windows.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//include Direct 3D header files
#include <d3dcompiler.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <d3d10.h>


// Global declarations


class D3D11Renderer : public IRenderer
{
public:
	D3D11Renderer();
	~D3D11Renderer();

	//functions from interface
	bool init(void *pWindowHandle, bool fullscreen);
	void clear(float r, float g, float b, float a);
	void present();
	void render();
private:
	bool createDevice(HWND pWindowHandle, int windowWidth, int windowHeight, bool fullScreen);		//Create back buffer
	bool createInitialRenderTarget(int windowWidth, int windowHeight);								//Bind texture to backbuffer/initalize viewport
	bool createVertexBuffer();																		//Create vertex buffer to hold vertex data
	bool createIndexBuffer();
	bool createConstantBuffer(int windowWidth, int windowHeight);
	bool initPipeline(void);																		//Bind vertex buffer and shaders to input assembler

	
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	//bool loadEffectFromMemory(const char* pMem);
	//bool loadEffectFromFile(char* pFilename);
	//void createCamera(XMVECTOR &position, XMVECTOR &focus, XMVECTOR &up, float fov, float aspectRatio, float nearClip, float farClip);
	//void positionObject(float x, float y, float z);

	//bool loadBaseTexture(char* pFilename);
private:
	ID3D11Device *m_pD3D11Device;								//pointer to Direct3D device interface
	ID3D11DeviceContext *m_pD3D11DeviceContext;					//pointer to Direct3D device context
	IDXGISwapChain *m_pSwapChain;								//pointer to swap chain interface

	ID3D11RenderTargetView  *m_pBackbuffer;						//pointer to object that holds info about render target
	ID3D11Buffer *pVBuffer;										//the vertex buffer
	ID3D11Buffer * m_pIndexBuffer;
	ID3D11Buffer *m_pConstantBuffer;

	ID3D11InputLayout *pInputLayout;							//the input layout
	
	ID3D11VertexShader *pVS;									//the vertex shader
	ID3D11PixelShader *pPS;										//the pixel shader

	DirectX::XMMATRIX m_World;
	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Projection;

};