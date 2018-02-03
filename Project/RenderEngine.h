#pragma once
#include "Drawable.h"
#include "Camera.h"
#include "DeferredShaders.h"
#include "IncludeDX11.h"
#include <Windows.h>
#include <vector>

/*

Själva motorn som ritar ut objekten 

*/

using namespace DirectX;

class RenderEngine
{ 
private:
	//Variables

	struct wvp_Matrixes
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

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

	//RT Krakens
	std::vector<ID3D11RenderTargetView*> RTViews;
	std::vector<ID3D11ShaderResourceView*> SRViews;
	std::vector<ID3D11Texture2D*> RTTextures;

	//backbuffer
	ID3D11Texture2D * back_buffer_texture;
	ID3D11RenderTargetView * back_buffer_view;

	ID3D11RasterizerState * RSState;

	ID3D11DepthStencilState * DSState;
	ID3D11Texture2D * depthStencil_texture;
	ID3D11DepthStencilView * DSView;

	D3D11_VIEWPORT view_port;

	ID3D11Buffer * vertex_buffer;
	ID3D11Buffer * matrix_cb;

	//shaders for deferred
	DeferredShaders * deferred_shading;
private:
	//functions
	bool createWindow(HINSTANCE hInstance, int nCmdShow);
	bool initiateEngine();
	bool setupRTVs();
	void setupVP();
	bool setupDepthStencilBuffer();
	bool setupRasterizer();
	bool createCBs();
	void mapCBs();
	void windowProc();
	void setupOMS();
	void setMatrixes(XMMATRIX &in_world, XMMATRIX &in_view, XMMATRIX &in_projection);
	void createVertexBuffer(const int vertex_buffer_size);

public:

	RenderEngine(HINSTANCE hInstance, int nCmdShow);
	~RenderEngine();

	void Draw(const Drawable* objectToRender); // draw called object
	void loadViewMatrix(XMFLOAT3X3 new_view);
	void loadProjectionMatrix(XMFLOAT3X3 new_projection);
};