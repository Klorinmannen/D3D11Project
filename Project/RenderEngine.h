#pragma once
#include "Drawable.h"
#include "Camera.h"
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
/*

Själva motorn som ritar ut objekten 

*/

using namespace DirectX;

class RenderEngine
{ 
private:
	//D3D11 data
	IDXGISwapChain * c_swapChain;
	ID3D11Device * c_device;
	ID3D11DeviceContext * c_deviceContext;
	std::vector<ID3D11RenderTargetView*> c_buffersRTV;

private:
	//Variables
	XMMATRIX xm_view;
	XMMATRIX xm_projection;
	XMMATRIX xm_orthoProj;
	XMFLOAT3X3 xmf_view;
	XMFLOAT3X3 xmf_projection;
	
	const int HEIGHT = 680;
	const int WIDTH = 680;
	int RTVs = 6;
	Camera camera;

	HWND windowHandle;

private:
	//functions
	void createShaders();
	bool createWindow(HINSTANCE hInstance, int nCmdShow);
	bool initiateEngine();
	bool setupRTVs();
	bool setupDepthStencil();
	bool setupRasterizer();
	bool setCBs();
	void windowProc();

public:

	RenderEngine(HINSTANCE hInstance, int nCmdShow);
	~RenderEngine();

	void Draw(Drawable* objectToRender); // draw called object
	void loadViewMatrix(XMFLOAT3X3 new_view);
	void loadProjectionMatrix(XMFLOAT3X3 new_projection);
};