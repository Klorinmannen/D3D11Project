#pragma once
#include "Drawable.h"
#include "Camera.h"
#include "VSShader.h"
#include "GSShader.h"
#include "PSShader.h"

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
	//Variables
	XMMATRIX xm_view;
	XMMATRIX xm_projection;
	XMMATRIX xm_orthoProj;
	XMFLOAT3X3 xmf_view;
	XMFLOAT3X3 xmf_projection;


	bool useRastBackCull = true;

	const int HEIGHT = 680;
	const int WIDTH = 680;
	const int VIEW_COUNT = 3;
	Camera camera;

	HWND windowHandle;
private:
	//D3D11 data
	IDXGISwapChain * swapChain;
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;

	std::vector<ID3D11RenderTargetView*> RTViews;
	std::vector<ID3D11ShaderResourceView*> SRViews;
	std::vector<ID3D11Texture2D*> RTTextures; //RenderTargetTextures

	ID3D11RasterizerState* RSState;
	ID3D11DepthStencilState* DSState;
	ID3D11Texture2D* depthStencil_texture;
	ID3D11DepthStencilView* DSView;
	D3D11_VIEWPORT view_port;

private:
	//functions
	void createShaders();
	bool createWindow(HINSTANCE hInstance, int nCmdShow);
	bool initiateEngine();
	bool setupRTVs();
	void setupVP();
	bool setupDepthStencilBuffer();
	bool setupRasterizer();
	bool setCBs();
	void windowProc();
	void setupOMS();

public:

	RenderEngine(HINSTANCE hInstance, int nCmdShow);
	~RenderEngine();

	void Draw(Drawable* objectToRender); // draw called object
	void loadViewMatrix(XMFLOAT3X3 new_view);
	void loadProjectionMatrix(XMFLOAT3X3 new_projection);
};