#include "D3D11Renderer.h"

using namespace DirectX;

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 colour;
};
struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};
D3D11Renderer::D3D11Renderer(){
	m_pD3D11Device = NULL;
	m_pSwapChain = NULL;
	m_pBackbuffer = NULL;
	m_pD3D11DeviceContext = NULL;
	pVBuffer = nullptr;										
	m_pIndexBuffer = nullptr;
	m_pConstantBuffer = nullptr;
	m_View = XMMatrixIdentity();
	m_Projection = XMMatrixIdentity();
}

D3D11Renderer::~D3D11Renderer()
{
	if (m_pD3D11Device)
		m_pD3D11Device->Release();
	if (m_pD3D11DeviceContext)
		m_pD3D11DeviceContext->Release();
	if (m_pSwapChain)
		m_pSwapChain->Release();
	if (m_pBackbuffer)
		m_pBackbuffer->Release();


}

bool D3D11Renderer::init(void *pWindowHandle, bool fullscreen){

	HWND window = (HWND)pWindowHandle;

	RECT windowRect;

	//Returns the size of the clients area in windowRect
	GetClientRect(window, &windowRect);

	UINT width = windowRect.right - windowRect.left;
	UINT height = windowRect.bottom - windowRect.top;

	//Calculate actual size of window (excludes borders/menus)
	//	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; //set size but not position
	//	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); //adjust the size


	//Calls functions in D3D10Renderer.
	if (!createDevice(window, width, height, fullscreen))
		return false;

	if (!createInitialRenderTarget(width, height))
		return false;

	if (!initPipeline())
		return false;

	if (!createVertexBuffer())
		return false;

	if (!createIndexBuffer())
		return false;

	if (!createConstantBuffer(width, height))
		return false;

	//if (!initPipeline())
		//return false;


	return true;

}
void D3D11Renderer::clear(float r, float g, float b, float a)
{

	const float ClearColor[4] = { r, g, b, a };
	m_pD3D11DeviceContext->ClearRenderTargetView(m_pBackbuffer, (ClearColor));

}
void D3D11Renderer::present()
{
	m_pSwapChain->Present(0, 0);
}
void D3D11Renderer::render(){
	// Update our time
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCur = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCur;
		t = (timeCur - timeStart) / 1000.0f;
	}

	//
	// Animate the cube
	//
	m_World = XMMatrixRotationY(t);
	

	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World);
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);
	m_pD3D11DeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	//set the shader objects
	m_pD3D11DeviceContext->VSSetShader(pVS, nullptr, 0);
	m_pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pD3D11DeviceContext->PSSetShader(pPS, nullptr, 0);

	//select which primitive type we are using
	//m_pD3D11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw vertex buffer to back buffer
	m_pD3D11DeviceContext->DrawIndexed(36, 0, 0);

}


bool D3D11Renderer::createDevice(HWND pWindowHandle, int windowWidth, int windowHeight, bool fullScreen)
{

		/*UINT createDeviceFlags = 0;
	#ifdef _DEBUG
	createDeviceFlags|=D3D10_CREATE_DEVICE_DEBUG;
	#endif*/


	//===============================================
	//		CREATE BACK BUFFER
	//===============================================
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));		//clear struct for use


	//Fill swap chain description struct
	if (fullScreen)
		scd.BufferCount = 2;
	else
		scd.BufferCount = 1;									//number of back buffers

	scd.OutputWindow = pWindowHandle;							//window where  swap chain will present image
	scd.Windowed = (BOOL)(!fullScreen);							//windowed/full screen mode
	scd.SampleDesc.Count = 1;									//how many multi samples
	scd.SampleDesc.Quality = 0;									//multisample quality level
	scd.BufferDesc.Width = windowWidth;							//Set back buffer width
	scd.BufferDesc.Height = windowHeight;						//Set back buffer height
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			//use 32 bit colour
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			//how swap chain is to be used
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;			//allow full screen switching (Alt-Enter)


	//Create a device, device context and swap chain using the info in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,													//Pointer to video adapter to be used (NULL for default)
		D3D_DRIVER_TYPE_HARDWARE,								//Driver type to create
		NULL,													//Handle to DLL for software rasterizer (NULL due to hardware)
		NULL,													//flags for runtime layers?
		NULL,													//Pointer to feature level array
		NULL,													//No of elements in feature level array
		D3D11_SDK_VERSION,										//SDK version
		&scd,													//pointer to swap chain description
		&m_pSwapChain,											//Pointer to Swap Chain object
		&m_pD3D11Device,										//Pointer to Device object
		NULL,													//Pointer to feature level
		&m_pD3D11DeviceContext); 							//Pointer to Device Context object
		
		
	if (FAILED(hr))
		return false;

	return true;

}
bool D3D11Renderer::createInitialRenderTarget(int windowWidth, int windowHeight)
{
	//============================
	//Bind texture to back buffer
	//============================
	ID3D11Texture2D* pBackBuffer = nullptr;
	
	HRESULT hr = S_OK;

	hr = m_pSwapChain->GetBuffer(
		0,									//no of back buffer to get
		__uuidof(ID3D11Texture2D),			//id no of ID3D11Texture2D
		(LPVOID*)&pBackBuffer);		//void pointer/texture object
		
	if (FAILED(hr))
		return false;

	

	//use back buffer address to create render target
	hr = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &m_pBackbuffer);

	if (FAILED(hr))
		return false;

	pBackBuffer->Release();

	//Set render target as back buffer
	m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pBackbuffer, NULL);

	//=========================================
	// Init Viewport
	//=========================================

	//Set Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = windowWidth;
	viewport.Height = windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//activates viewport structs

	m_pD3D11DeviceContext->RSSetViewports(1,	//no of viewports
		&viewport);								//list of pointers to viewport structs


	return true;
}
bool D3D11Renderer::createVertexBuffer()
{


	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
	};

	UINT numVertices = ARRAYSIZE(vertices);


	//create vertex buffer object
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;						//write access by CPU and GPU
	bd.ByteWidth = sizeof(SimpleVertex) * 8;			//size is vertex struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//use as vertex buffer
	bd.CPUAccessFlags = 0;								//allow CPU to write in buffer
	bd.MiscFlags = 0;

	//Actual data copied to vertex buffer
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	InitData.pSysMem = vertices;

	HRESULT hr = m_pD3D11Device->CreateBuffer(&bd, &InitData, &pVBuffer);

	if (FAILED(hr))
	{
		OutputDebugString(L"Can't create buffer");
		return false;
	}

	//Select which vertex buffer to display
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	//Bind vertex buffer to device
	m_pD3D11DeviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);






	return true;
}

bool D3D11Renderer::createIndexBuffer()
{
	//Create the index buffer
	WORD indices[] =
	{
		3, 1, 0,
		2, 1, 3,

		0, 5, 4,
		1, 5, 0,

		3, 4, 7,
		0, 4, 3,

		1, 6, 5,
		2, 6, 1,

		2, 7, 6,
		3, 7, 2,

		6, 4, 5,
		7, 4, 6,
	};

	UINT numIndices = ARRAYSIZE(indices);

	//create index buffer object
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;						//write access by CPU and GPU
	bd.ByteWidth = sizeof(WORD) * 36;			//size is vertex struct * 3
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;			//use as vertex buffer
	bd.CPUAccessFlags = 0;								//allow CPU to write in buffer
	bd.MiscFlags = 0;

	//Actual data copied to Index buffer
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	InitData.pSysMem = indices;

	HRESULT hr = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);

	if (FAILED(hr))
	{
		OutputDebugString(L"Can't create index buffer");
		return false;
	}

	m_pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// Set primitive topology
	m_pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	return true;

}
bool D3D11Renderer::createConstantBuffer(int windowWidth, int windowHeight)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;						//write access by CPU and GPU
	bd.ByteWidth = sizeof(ConstantBuffer);			//size is vertex struct * 3
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			//use as vertex buffer
	bd.CPUAccessFlags = 0;								//allow CPU to write in buffer
	bd.MiscFlags = 0;

	HRESULT hr = m_pD3D11Device->CreateBuffer(&bd, NULL, &m_pConstantBuffer);

	if (FAILED(hr))
	{
		OutputDebugString(L"Can't create constant buffer");
		return false;
	}

	//Initalise the world matrix
	m_World = XMMatrixIdentity();

	//Initalise the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	//Initalise the Projection matrix
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / (FLOAT)windowHeight, 0.01f, 100.0f);

	return true;
}
////Function to load shaders and apply to pipeline
bool D3D11Renderer::initPipeline(void){
	//Prepares GPU for rendering

	//=============================
	//load and compile two shaders
	//=============================
	ID3DBlob *pVSBlob = nullptr;			//blob holds compiled shader
	ID3DBlob *pPSBlob = nullptr;
	HRESULT hr = S_OK;

	hr = CompileShaderFromFile(L"Tutorial04.fx",
		"VS",
		"vs_4_0",
		&pVSBlob);


	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	
		return false;
	}

	hr = CompileShaderFromFile(L"Tutorial04.fx",
		"PS",
		"ps_4_0",
		&pPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	
		return false;
	}


	//encapsulate both shaders into shader objects
	hr = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVS);
	
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return false;
	}
	
	hr = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPS);
	
	if (FAILED(hr))
	{
		pPSBlob->Release();
		return false;
	}

	

	//set the shader objects
	//m_pD3D11DeviceContext->VSSetShader(pVS, 0, 0);
	//m_pD3D11DeviceContext->PSSetShader(pPS, 0, 0);

	//===============================
	//create the input layout object
	//===============================

	//D3D11_INPUT_ELEMENT_DESC ied = {
	//SemanticName - Nature of element
	//SemanticIndex - Used if multiples
	//Format - data type used for element
	//InputSlot  - Can pass multiple vertex buffers to GPU at once 0 - 15
	//AlignedByteOffset - byte offset of element
	//InputSlotClass - used for instanceing 
	//InstanceDataStepRate - Used for instancing
	//}

	//Define layout structure
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(ied);

	//Create the input layout
	hr = m_pD3D11Device->CreateInputLayout(ied, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pInputLayout);
	
	pVSBlob->Release();
	pPSBlob->Release();

	if (FAILED(hr))
		return false;
	

	//Bind input layout to Input assembler
	m_pD3D11DeviceContext->IASetInputLayout(pInputLayout);


	return true;
}

HRESULT D3D11Renderer::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob = nullptr;

	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

