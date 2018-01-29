//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//	   - modified by FLL
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <chrono>
#include <thread>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <vector>
#include "bth_image.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define SCREEN_WIDTH 640.0f
#define SCREEN_HIEGHT 480.0f

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT CreateDirect3DContext(HWND wndHandle);

using namespace DirectX;

struct VERTEXES
{
	XMFLOAT3 v0 = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	XMFLOAT3 v1 = XMFLOAT3(0.5f, 0.5f, 0.0f);
	XMFLOAT3 v2 = XMFLOAT3(0.5f, -0.5f, 0.0f);
	XMFLOAT3 v3 = XMFLOAT3(-0.5f, -0.5f, 0.0f);
};

VERTEXES vertices;

struct TEX
{
	XMFLOAT2 t0 = XMFLOAT2(0.0f, 0.0f); // v0
	XMFLOAT2 t1 = XMFLOAT2(1.0f, 0.0f); // v1
	XMFLOAT2 t2 = XMFLOAT2(1.0f, 1.0f); // v2
	XMFLOAT2 t3 = XMFLOAT2(0.0f, 1.0f); // v3
};

TEX tex;

struct light
{
	XMFLOAT4 pos;
	XMFLOAT4 color;
};

light newLight;


struct VS_CONSTANT_BUFFER
{
	DirectX::XMFLOAT4X4 objectToWorldM;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4X4 worldToViewM;
	DirectX::XMFLOAT4X4 scale;
	DirectX::XMFLOAT4X4 wvp;
	XMFLOAT4X4 wv;
};

VS_CONSTANT_BUFFER vertexCB;
float rotation = 0.00f;

XMMATRIX rotY = XMMatrixRotationY(rotation);
XMMATRIX projM = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, SCREEN_WIDTH / SCREEN_HIEGHT, 0.1f, 20.0f);
XMVECTOR camPos = XMVectorSet(0, 0, -2, 0);
XMVECTOR lookAt = XMVectorSet(0, 0, 0, 0);
XMVECTOR up = XMVectorSet(0, 1, 0, 0);

XMMATRIX view = XMMatrixLookAtLH(camPos, lookAt, up);



// Most directX Objects are COM Interfaces
// https://es.wikipedia.org/wiki/Component_Object_Model
IDXGISwapChain* gSwapChain = nullptr;

// Device and DeviceContext are the most common objects to
// instruct the API what to do. It is handy to have a reference
// to them available for simple applications.
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
ID3D11Texture2D* pBackBuffer = nullptr;

// A "view" of a particular resource (the color buffer)
ID3D11RenderTargetView* gBackbufferRTV = nullptr;

// a resource to store Vertices in the GPU
ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11Buffer* gConstantBuffer = nullptr;
ID3D11Buffer* indexBuffer = nullptr;

ID3D11Buffer* cLight = nullptr;

ID3D11RasterizerState* rasterizerState = nullptr;
ID3D11DepthStencilState* depthBufferState = nullptr;
ID3D11DepthStencilView* depthStencilView = nullptr;
ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11Texture2D* depthTexture2D = nullptr;

ID3D11ShaderResourceView* bthTextureView = nullptr;
ID3D11Texture2D* bthTexture2D = nullptr;

// resources that represent shaders
ID3D11VertexShader* gVertexShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;
ID3D11GeometryShader* gGeometryShader = nullptr;

void createConstantBuffer();
void rotate();
void rasterizer();
void depthBuffer();
void bthTexture();
void createLightSource();

HRESULT CreateShaders()
{
	// Binary Large OBject (BLOB), for compiled shader, and errors.
	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	HRESULT result = D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (pVS)
			pVS->Release();
		return result;
	}

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);
	// create input layout (verified using vertex shader)
	// Press F1 in Visual Studio with the cursor over the datatype to jump
	// to the documentation online!
	// please read:
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb205117(v=vs.85).aspx
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{
			"POSITION",		// "semantic" name in shader
			0,				// "semantic" index (not used)
			DXGI_FORMAT_R32G32B32_FLOAT, // size of ONE element (3 floats)
			0,							 // input slot
			0,							 // offset of first element
			D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
			0							 // used for INSTANCING (ignore)
		},
		{
			"TEXCOORD",
			0,				// same slot as previous (same vertexBuffer)
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			12,							// offset of FIRST element (after POSITION)
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

	};

	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);

	// we do not need anymore this COM object, so we release it.
	pVS->Release();



	//create gshader
	result = D3DCompileFromFile(
		L"Geo.hlsl",	 // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"GS_main",		// entry point
		"gs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (pVS)
			pVS->Release();
		return result;
	}


	gDevice->CreateGeometryShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gGeometryShader);
	pVS->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"Fragment.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		&errorBlob			// pointer for Error Blob messages.
	);

	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (pPS)
			pPS->Release();
		return result;
	}

	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();

	return S_OK;

}

ID3D11Buffer* CreateAppendConsumeBuffer()
{
	return 0;
}


void CreateTriangleData()
{

	struct TriangleVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 Tex;
	};




	// Array of Structs (AoS)
	TriangleVertex triangleVertices[4] =
	{
		DirectX::XMFLOAT3(vertices.v0),	//v0 pos x, y, z
		DirectX::XMFLOAT2(tex.t0),	//v0 color

		DirectX::XMFLOAT3(vertices.v1),	//v1
		DirectX::XMFLOAT2(tex.t1),	//v1 color

		DirectX::XMFLOAT3(vertices.v2), //v2
		DirectX::XMFLOAT2(tex.t2),	//v2 color

		DirectX::XMFLOAT3(vertices.v3), //v3
		DirectX::XMFLOAT2(tex.t3),
	};
	
	
	int indices[6] = {2, 1, 0, 2, 0, 3};


	D3D11_BUFFER_DESC indexDesc;
	memset(&indexDesc, 0, sizeof(indexDesc)); //vad gör denna?
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.ByteWidth = sizeof(indices);

	D3D11_SUBRESOURCE_DATA iData;
	iData.pSysMem = indices;
	

	// Describe the Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc)); 
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; 	// what type of buffer will this be?
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;		// what type of usage (press F1, read the docs)
	bufferDesc.ByteWidth = sizeof(triangleVertices); 	// how big in bytes each element in the buffer is.

	// this struct is created just to set a pointer to the
	// data containing the vertices.
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;

	// create a Vertex Buffer
	gDevice->CreateBuffer(&bufferDesc, &data, &gVertexBuffer);
	gDevice->CreateBuffer(&indexDesc, &iData, &indexBuffer);
}

void SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = SCREEN_WIDTH;
	vp.Height = SCREEN_HIEGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}

void Render()
{
	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 0, 1 };

	// use DeviceContext to talk to the API
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	// specifying NULL or nullptr we are disabling that stage
	// in the pipeline
	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
	gDeviceContext->PSSetShaderResources(0, 1, &bthTextureView);

	UINT32 vertexSize = sizeof(float) * 5;
	UINT32 offset = 0;
	
	gDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// specify which vertex buffer to use next.
	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	
	// specify the topology to use when drawing
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// specify the IA Layout (how is data passed)
	gDeviceContext->IASetInputLayout(gVertexLayout);

	// issue a draw call of 3 vertices (similar to OpenGL)
	gDeviceContext->DrawIndexed(6, 0, 0);
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster
	
	if (wndHandle)
	{
		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context

		SetViewport(); //3. Sätt viewport

		CreateShaders(); //4. Skapa vertex- och pixel-shaders

		createConstantBuffer();
		createLightSource();
		rasterizer();

		bthTexture();

		CreateTriangleData(); //5. Definiera triangelvertiser, 6. Skapa vertex buffer, 7. Skapa input layout

		ShowWindow(wndHandle, nCmdShow);
		
		auto frameTimer = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::ratio<1, 80>> duration; // 80 fps
		while (WM_QUIT != msg.message)
		{
			
			
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Render(); //8. Rendera

				gSwapChain->Present(0, 0); //9. Växla front- och back-buffer
			}
			
			duration = std::chrono::high_resolution_clock::now() - frameTimer;

			if (duration.count() >= 1)
			{
				rotate();
				frameTimer = std::chrono::high_resolution_clock::now();
			}


		}


		bthTextureView->Release();
		cLight->Release();
		gVertexBuffer->Release();
		gVertexLayout->Release();
		gVertexShader->Release();
		gGeometryShader->Release();
		gPixelShader->Release();
		gConstantBuffer->Release();
		indexBuffer->Release();
		depthStencilView->Release();
		gBackbufferRTV->Release();
		gSwapChain->Release();
		gDevice->Release();
		gDeviceContext->Release();
		DestroyWindow(wndHandle);
	}

	return (int) msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.hInstance      = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HIEGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_D3D_DEMO",
		L"BTH Direct3D Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	

	return handle;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;		
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT CreateDirect3DContext(HWND wndHandle)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

	// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);



	if (SUCCEEDED(hr))
	{
		depthBuffer();
		// get the address of the back buffer
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();

		// set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(2, &gBackbufferRTV, depthStencilView);
	}
	return hr;
	
}

void createConstantBuffer()
{
	HRESULT result;

/*
	rotY = XMMatrixTranspose(rotY);
	view = XMMatrixTranspose(view);
	projM = XMMatrixTranspose(projM);*/

	XMStoreFloat4x4(&vertexCB.objectToWorldM, rotY);
	XMStoreFloat4x4(&vertexCB.worldToViewM, view);
	XMStoreFloat4x4(&vertexCB.projectionMatrix, projM);

	XMStoreFloat4x4(&vertexCB.wvp, rotY * view * projM);
	XMStoreFloat4x4(&vertexCB.wv, view * rotY);

	vertexCB.scale = XMFLOAT4X4(1, 0, 0, 0,
								0, 1, 0, 0,
								0, 0, 1, 0,
								0, 0, 0, 1);
	

	D3D11_BUFFER_DESC descCB;
	descCB.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	descCB.Usage = D3D11_USAGE_DYNAMIC;
	descCB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	descCB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	descCB.MiscFlags = 0;
	descCB.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vertexCB;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	result = gDevice->CreateBuffer(&descCB, &data, &gConstantBuffer);

	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
}

void rotate()
{
		rotation += 0.005f;
		XMMATRIX rotY = XMMatrixRotationY(rotation);

		XMStoreFloat4x4(&vertexCB.wvp, rotY * view * projM);
		XMStoreFloat4x4(&vertexCB.wv, view * rotY);
		XMStoreFloat4x4(&vertexCB.objectToWorldM, rotY);


		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		gDeviceContext->Map(gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &vertexCB, sizeof(vertexCB));
		gDeviceContext->Unmap(gConstantBuffer, 0);

		if (rotation >= 6.283185312)
		{
			rotation = 0.0f;
		}
}

void rasterizer()
{

	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.AntialiasedLineEnable = FALSE;
	rastDesc.DepthBias = 0;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.DepthClipEnable = TRUE;
	rastDesc.FrontCounterClockwise = TRUE;
	rastDesc.MultisampleEnable = FALSE;
	rastDesc.ScissorEnable = FALSE;
	rastDesc.SlopeScaledDepthBias = 0.0f;

	gDevice->CreateRasterizerState(&rastDesc, &rasterizerState);
	gDeviceContext->RSSetState(rasterizerState);
	rasterizerState->Release();
}

void depthBuffer()
{
	HRESULT result;
	D3D11_DEPTH_STENCIL_DESC depthDesc; //default values
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	//stencil operating of frontfacing
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Stencil operation if backfacing
	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//depthTestParameters
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

	//StencilTestParameters
	depthDesc.StencilEnable = FALSE;
	depthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	result = gDevice->CreateDepthStencilState(&depthDesc, &depthBufferState);

	D3D11_TEXTURE2D_DESC texture2DDesc;
	ZeroMemory(&texture2DDesc, sizeof(texture2DDesc));
	texture2DDesc.Width = (int)SCREEN_WIDTH;
	texture2DDesc.Height = (int)SCREEN_HIEGHT;
	texture2DDesc.MipLevels = 1;
	texture2DDesc.ArraySize = 1;
	texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2DDesc.SampleDesc.Count = 1;
	texture2DDesc.SampleDesc.Quality = 0;
	texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2DDesc.CPUAccessFlags = 0;
	texture2DDesc.MiscFlags = 0;
	
	result = gDevice->CreateTexture2D(&texture2DDesc, NULL, &depthTexture2D);


	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	ZeroMemory(&DSVDesc, sizeof(DSVDesc));
	DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0;
	DSVDesc.Flags = 0;

	gDeviceContext->OMSetDepthStencilState(depthBufferState, 1);
	depthBufferState->Release();
	result = gDevice->CreateDepthStencilView(depthTexture2D, &DSVDesc, &depthStencilView);
	depthTexture2D->Release();

}

void bthTexture()
{

	HRESULT result;

	D3D11_TEXTURE2D_DESC bthTextDesc;
	bthTextDesc.Width = BTH_IMAGE_WIDTH;
	bthTextDesc.Height = BTH_IMAGE_HEIGHT;
	bthTextDesc.MipLevels = bthTextDesc.ArraySize = 1;
	bthTextDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bthTextDesc.SampleDesc.Count = 1;
	bthTextDesc.SampleDesc.Quality = 0;
	bthTextDesc.Usage = D3D11_USAGE_DEFAULT;
	bthTextDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bthTextDesc.MiscFlags = 0;
	bthTextDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA txtureData;
	txtureData.pSysMem = (void*)BTH_IMAGE_DATA;
	txtureData.SysMemPitch = BTH_IMAGE_WIDTH * 4 * sizeof(char);
	result = gDevice->CreateTexture2D(&bthTextDesc, &txtureData, &bthTexture2D);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;
	shaderViewDesc.Format = bthTextDesc.Format;
	shaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderViewDesc.Texture2D.MipLevels = bthTextDesc.MipLevels;
	shaderViewDesc.Texture2D.MostDetailedMip = 0;
	result = gDevice->CreateShaderResourceView(bthTexture2D, &shaderViewDesc, &bthTextureView);
	bthTexture2D->Release();

}

void createLightSource()
{

	HRESULT result;
	//XMVECTOR temp = XMVectorSet(0, 0, -3, 1);
	//XMStoreFloat4(&newLight.pos, XMVector4Transform(temp, view));
	newLight.pos = XMFLOAT4(0.0f, 0.0f, -3.0f, 1.0f);
	newLight.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 7.0f);
	

	D3D11_BUFFER_DESC lightDesc;
	lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightDesc.ByteWidth = sizeof(light);
	lightDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightDesc.MiscFlags = 0;
	lightDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA lightData;
	lightData.pSysMem = &newLight;
	lightData.SysMemPitch = 0;
	lightData.SysMemSlicePitch = 0;

	result = gDevice->CreateBuffer(&lightDesc, &lightData, &cLight);

	gDeviceContext->PSSetConstantBuffers(0, 2, &cLight);
	gDeviceContext->PSSetConstantBuffers(1, 2, &gConstantBuffer);

}
