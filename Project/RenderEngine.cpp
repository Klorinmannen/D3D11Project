#include "RenderEngine.h"

void RenderEngine::createShaders()
{
	/*
	compile shaders etc etc

	*/
}

bool RenderEngine::createWindow(HINSTANCE hInstance, int nCmdShow)
{
	//MSDN example creating a window
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(wcx);				//Size of struct
	wcx.style = CS_HREDRAW | CS_VREDRAW;	//Redraw is size changes
	wcx.lpfnWndProc = this->windowProc;		//A window procedure
	wcx.hInstance = hInstance;				//Handle to program instance
	wcx.lpszClassName = "D3D11Project";		//Name of windows class

	if (!RegisterClassEx(&wcx))
	{
		return false;
	}

	this->windowHandle = CreateWindow(  "D3D11Project",
										"D3D11Project",
										WS_OVERLAPPEDWINDOW,
										CW_USEDEFAULT,
										CW_USEDEFAULT,
										this->WIDTH,
										this->HEIGHT,
										NULL,
										NULL,
										hInstance,
										NULL);
	if (!this->windowHandle)
	{
		return false;
	}

	ShowWindow(this->windowHandle, nCmdShow);
}

bool RenderEngine::initiateEngine()
{
	/* 
	create swapchain
	create device
	create context
	*/

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.BufferCount = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = this->windowHandle;
	desc.SampleDesc.Count = 1;
	desc.Windowed = TRUE;

	HRESULT hresult = D3D11CreateDeviceAndSwapChain(NULL,
													D3D_DRIVER_TYPE_HARDWARE,
													NULL,
													D3D11_CREATE_DEVICE_DEBUG,
													NULL,
													NULL,
													D3D11_SDK_VERSION,
													&desc,
													&this->c_swapChain,
													&this->c_device,
													NULL,
													&this->c_deviceContext);
	if (FAILED(hresult))
	{
		return false;
	}

	this->setupRTVs();
	this->setupDepthStencil();
	this->setupRasterizer();

}

bool RenderEngine::setupRTVs()
{
	ID3D11Texture2D* tempText;

	this->c_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&tempText);

	this->c_device->CreateRenderTargetView(tempText);

	return false;
}

bool RenderEngine::setupDepthStencil()
{
	return false;
}


RenderEngine::RenderEngine(HINSTANCE hInstance, int nCmdShow)
{

	this->initiateEngine();

	this->createWindow(hInstance, nCmdShow);
}

RenderEngine::~RenderEngine()
{
}

void RenderEngine::Draw(Drawable * objectToRender)
{
	/*
	draw depending on object
	*/



}
