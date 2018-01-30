#pragma once
#include "Drawable.h"
#include "Camera.h"
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
/*

Själva motorn som ritar ut objekten 

*/

using namespace DirectX;

class RenderEngine
{ 
private:
	//D3D11 Stuff
	IDXGISwapChain * c_swapChain;
	ID3D11Device * c_device;
	ID3D11DeviceContext * c_deviceContext;

private:
	//Variables
	XMMATRIX xm_view;
	XMMATRIX xm_projection;
	XMFLOAT3X3 xmf_view;
	XMFLOAT3X3 xmf_projection;
	
	Camera camera;

	HWND wndHandle;

private:
	//functions
	void createShaders();
	void createWindow();
	void initiate();

public:

	RenderEngine();
	~RenderEngine();

	void Draw(Drawable* objectToRender); // draw called object
	void loadViewMatrix(XMFLOAT3X3 new_view);
	void loadProjectionMatrix(XMFLOAT3X3 new_projection);
};