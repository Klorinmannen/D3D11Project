#ifndef RenderEngine_H
#define RenderEngine_H

#include "Geometry.h"
#include "Terrain.h"
#include "Settings.h"
#include "Camera.h"
#include "DeferredShaders.h"
#include <windows.h>
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

	bool useRastBackCull = false;

	int HEIGHT;
	int WIDTH;
	float nearZ = 0.1f;
	float farZ = 1000.0f;
	const int VIEW_COUNT = 2;

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

	float black[4] = {0 , 0, 0, 1};

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
	bool initiateEngine(HWND handle);
	bool setupRTVs();
	void setupVP();
	bool setupDepthStencilBuffer();
	bool setupRasterizer();
	bool createCBs();
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void setupOMS();


	//Render functions
	void updateMatrixes();
	void updateShaders(int in_pass);
	void updateBuffers(ID3D11Buffer* in_VertexBuffer, ID3D11Buffer* in_IndexBuffer, float size_of);
	void clearRT();
	void mapCBs();
	void layoutTopology(int in_topology, int in_layout);
	void setDrawCall(int nr_verticies);
public:

	RenderEngine(HWND wndHandle, HINSTANCE hInstance, int WIDTH, int HEIGHT);
	~RenderEngine();

	void setWorldMatrix(XMMATRIX world);
	void update();
	void Draw(Terrain * in_terrain); // draw called object
	void Draw(Geometry * in_geometry);

	ID3D11Device* getDevice();
};

#endif