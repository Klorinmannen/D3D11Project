#pragma once
#ifndef RenderEngine_H
#define RenderEngine_H

#include"IncludeDX11.h"
#include "Geometry.h"
#include "Camera.h"
#include "DeferredShaders.h"
#include <Windows.h>
#include <vector>
#include "Light.h"

/*

Engine using deferred rendering

*/

using namespace DirectX;

class RenderEngine
{ 
private:
	//Variables

	bool useRastBackCull = true;

	const int HEIGHT = 680;
	const int WIDTH = 680;
	const int VIEW_COUNT = 3;

	enum pass{Geometry_pass, Lightning_pass};

	struct matrix_wvp
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX wvp;
	};

	matrix_wvp m_wvp;
	//FPS
	Camera camera;
	//deferred shaders
	DeferredShaders * deferred_shading;
	Light lights;


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
	ID3D11DepthStencilView * depthStencilView;

	D3D11_VIEWPORT view_port;

	ID3D11Buffer * cb_lights;	
	ID3D11Buffer * cb_matrixes;

private:
	//start-up functions
	bool createWindow(HINSTANCE hInstance, int nCmdShow);
	bool initiateEngine();
	bool setupRTVs();
	void setupVP();
	bool setupDepthStencilBuffer();
	bool setupRasterizer();
	bool createCBs();
	LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void setupOMS();
	void setMatrixes();

	//Render functions
	void updateMatrixes();
	void updateShaders(int in_pass);
	void updateBuffers(ID3D11Buffer* in_VertexBuffer, ID3D11Buffer* in_IndexBuffer, float size_of);
	void clearRT();
	void mapCBs();
	void layoutTopology(int in_topology, int in_layout);
	void setDrawCall(int nr_verticies);
public:

	RenderEngine(HINSTANCE hInstance, int nCmdShow);
	~RenderEngine();

	void Draw(Terrain * in_terrain); // draw called object
	void Draw(Geometry * in_geometry);

};

#endif