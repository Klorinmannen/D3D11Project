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

	return true;
}

bool RenderEngine::initiateEngine()
{
	/* 
	create swapchain
	create device
	create context
	*/

	DXGI_SWAP_CHAIN_DESC bb_desc;
	ZeroMemory(&bb_desc, sizeof(bb_desc));

	bb_desc.BufferCount = 1;
	bb_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bb_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	bb_desc.OutputWindow = this->windowHandle;
	bb_desc.SampleDesc.Count = 1;
	bb_desc.Windowed = TRUE;

	HRESULT hresult = D3D11CreateDeviceAndSwapChain(NULL,
													D3D_DRIVER_TYPE_HARDWARE,
													NULL,
													D3D11_CREATE_DEVICE_DEBUG,
													NULL,
													NULL,
													D3D11_SDK_VERSION,
													&bb_desc,
													&this->swapChain,
													&this->device,
													NULL,
													&this->deviceContext);
	if (FAILED(hresult))
	{
		return false;
	}

	if (this->setupRTVs() && this->setupDepthStencilBuffer())
	{
		//if shit goes haywire
		return false;
	}
	this->setupRasterizer();
	this->setupVP();
	this->setupOMS();

}

bool RenderEngine::setupRTVs()
{
	//render&shader views
	D3D11_TEXTURE2D_DESC texture_desc;
	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;


	//wnds msgs
	HRESULT hResult;

	//create textures
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476253(v=vs.85).aspx << D3D11_TEXTURE2D_DESC
	ZeroMemory(&texture_desc, sizeof(texture_desc));
	texture_desc.Width = this->WIDTH;
	texture_desc.Height = this->HEIGHT;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1; //one texture in the array
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx << DXGI_FORMAT
	texture_desc.SampleDesc.Count = 1; // https://msdn.microsoft.com/en-us/library/windows/desktop/bb173072(v=vs.85).aspx << DXGI_SAMPLE_DESC
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; //Use as render targer and shader resource
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	//create all necessary textures (normals, position, texture color)
	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		hResult = this->device->CreateTexture2D(&texture_desc, NULL, &this->RTTextures[i]);
		if (FAILED(hResult))
		{
			return false;
		}
	}


	//create render target views
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476201(v=vs.85).aspx << D3D11_RENDER_TARGET_VIEW_DESC
	ZeroMemory(&rtv_desc, sizeof(rtv_desc));
	rtv_desc.Format = texture_desc.Format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0; // https://msdn.microsoft.com/en-us/library/windows/desktop/ff476244(v=vs.85).aspx << D3D11_TEX2D_RTV

	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		hResult = this->device->CreateRenderTargetView(this->RTTextures[i], &rtv_desc, &this->RTViews[i]);
		if (FAILED(hResult))
		{
			return false;
		}
	}

	//create shader resource views
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476211(v=vs.85).aspx << D3D11_SHADER_RESOURCE_VIEW_DESC
	ZeroMemory(&srv_desc, sizeof(srv_desc));
	srv_desc.Format = texture_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1; // https://msdn.microsoft.com/en-us/library/windows/desktop/ff476245(v=vs.85).aspx << D3D11_TEX2D_SRV
	srv_desc.Texture2D.MostDetailedMip = 0;

	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		hResult = this->device->CreateShaderResourceView(this->RTTextures[i], &srv_desc, &this->SRViews[i]);
		if (FAILED(hResult))
		{
			return false;
		}
	}

	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476519(v=vs.85).aspx << CreateShaderResourceView
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476517(v=vs.85).aspx << CreateRenderTargetView
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476521(v=vs.85).aspx << CreateTexture2D

	return true;
}

void RenderEngine::setupVP()
{
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476260(v=vs.85).aspx << D3D11_VIEWPORT

	this->view_port.Height = static_cast<float>(this->HEIGHT);
	this->view_port.Width = static_cast<float>(this->WIDTH);
	this->view_port.MinDepth = 0.0f;
	this->view_port.MaxDepth = 1.0f;
	this->view_port.TopLeftX = 0.0f;
	this->view_port.TopLeftY = 0.0f;
}

bool RenderEngine::setupDepthStencilBuffer()
{
	//depth descriptions
	D3D11_TEXTURE2D_DESC depth_texture_desc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	D3D11_DEPTH_STENCIL_DESC ds_desc;
	HRESULT hResult;



	//stencil operating of frontfacing
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476110(v=vs.85).aspx << D3D11_DEPTH_STENCIL_DESC
	ZeroMemory(&ds_desc, sizeof(ds_desc));

	ds_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	ds_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Stencil operation if backfacing
	ds_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	ds_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//depthTestParameters
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS;

	//StencilTestParameters
	ds_desc.StencilEnable = FALSE;
	ds_desc.DepthEnable = TRUE;
	ds_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	ds_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	hResult = this->device->CreateDepthStencilState(&ds_desc, &this->DSState);
	if (FAILED(hResult))
	{
		return false;
	}

	//create depth texture description, same as the rtv one
	ZeroMemory(&depth_texture_desc, sizeof(depth_texture_desc));
	depth_texture_desc.Width = this->WIDTH;
	depth_texture_desc.Height = this->HEIGHT;
	depth_texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_texture_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_texture_desc.MipLevels = 1;
	depth_texture_desc.ArraySize = 1;
	depth_texture_desc.CPUAccessFlags = 0;
	depth_texture_desc.MiscFlags = 0;

	hResult = this->device->CreateTexture2D(&depth_texture_desc, NULL, &this->depthStencil_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	//Create view of depthStencil
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476112(v=vs.85).aspx << D3D11_DEPTH_STENCIL_VIEW_DESC
	ZeroMemory(&dsv_desc, sizeof(dsv_desc));
	dsv_desc.Format = depth_texture_desc.Format;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;
	dsv_desc.Flags = 0;

	hResult = this->device->CreateDepthStencilView(this->depthStencil_texture, &dsv_desc, &this->DSView);
	if (FAILED(hResult))
	{
		return false;
	}

	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476507(v=vs.85).aspx << CreateDepthStencilView
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476521(v=vs.85).aspx << CreateTexture2D

	return true;
}

bool RenderEngine::setupRasterizer()
{
	D3D11_RASTERIZER_DESC rast_desc;
	HRESULT hResult;

	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx << D3D11_RASTERIZER_DESC

	ZeroMemory(&rast_desc, sizeof(rast_desc));
	rast_desc.FillMode = D3D11_FILL_SOLID;
	rast_desc.AntialiasedLineEnable = FALSE;
	rast_desc.DepthBias = 0;
	rast_desc.DepthBiasClamp = 0.0f;
	rast_desc.DepthClipEnable = TRUE;
	rast_desc.FrontCounterClockwise = TRUE;
	rast_desc.MultisampleEnable = FALSE;
	rast_desc.ScissorEnable = FALSE;
	rast_desc.SlopeScaledDepthBias = 0.0f;

	if (this->useRastBackCull)
	{
		rast_desc.CullMode = D3D11_CULL_BACK;
	}
	else
	{
		rast_desc.CullMode = D3D11_CULL_NONE;
	}
	
	hResult = this->device->CreateRasterizerState(&rast_desc, &this->RSState);
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

void RenderEngine::setupOMS()
{
	this->deviceContext->RSSetState(this->RSState);
	this->deviceContext->OMSetDepthStencilState(this->DSState, 1);
	this->deviceContext->OMSetRenderTargets(this->VIEW_COUNT, this->RTViews.data(), this->DSView); 
	this->deviceContext->RSSetViewports(1, &this->view_port);
}

RenderEngine::RenderEngine(HINSTANCE hInstance, int nCmdShow)
{
	this->swapChain = nullptr;
	this->device = nullptr;
	this->deviceContext = nullptr;

	//initiate vectors
	ID3D11Texture2D* t_entry = nullptr;
	ID3D11ShaderResourceView* srv_entry = nullptr;
	ID3D11RenderTargetView* rtv_entry = nullptr;
	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		this->RTTextures.push_back(t_entry);
		this->RTViews.push_back(rtv_entry);
		this->SRViews.push_back(srv_entry);
	}
	
	
	this->initiateEngine();
	this->createWindow(hInstance, nCmdShow);
}

RenderEngine::~RenderEngine()
{
	/*
	Empty all vectors/delete contents
	Release all com-objects

	*/
}

void RenderEngine::Draw(Drawable * objectToRender)
{
	/*
	draw depending on object
	set buffers 
	set topology
	etc etc
	*/

}
