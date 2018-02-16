#ifndef RenderEngine_H
#define RenderEngine_H

#include "Quad.h"
#include "Geometry.h"
#include "Terrain.h"
#include "Settings.h"
#include "Camera.h"
#include "DeferredShaders.h"
#include <windows.h>
#include <vector>
#include "Light.h"
#include "ObjLoader.h"
#include "Box.h"

/*

Engine using deferred rendering

*/

using namespace DirectX;

class RenderEngine
{ 
private:
	//Variables
	int debugFlag = 0;
	enum drawType { Indexed, NonIndexed };

	bool useRastBackCull = true;

	int HEIGHT;
	int WIDTH;
	float nearZ = 0.1f;
	float farZ = 10000.0f;
	const int VIEW_COUNT = 3;

	enum pass{Geometry_pass, Lightning_pass};

	//Transformation matrixes
	struct matrix_wvp
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX vp;
		XMMATRIX wvp;
	};
	matrix_wvp m_wvp;


	Quad quad;
	ObjLoader objLoader;
	Camera camera;
	DeferredShaders deferred_shading;
	Light lights;


	float black[4];

private:
	//D3D11 data

	IDXGISwapChain * swapChain;
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;

	//RT Krakens
	std::vector<ID3D11RenderTargetView*> RTViews;
	std::vector<ID3D11ShaderResourceView*> SRViews;
	std::vector<ID3D11Texture2D*> RTTextures;

	ID3D11ShaderResourceView* null[2] = { nullptr, nullptr };

	ID3D11ShaderResourceView * sampTexture;

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

	void loaderTest();

	//start-up functions
	bool initiateEngine(HWND handle);
	bool setupRTVs();
	void setupVP();
	bool setupDepthStencilBuffer();
	bool setupRasterizer();
	bool createCBs();
	void setupOMS();
	void setMatrixes();

	//Render functions
	void updateMatrixes(const XMMATRIX &in_world);
	void updateShaders(int in_pass);
	void updateBuffers(ID3D11Buffer* in_VertexBuffer, ID3D11Buffer* in_IndexBuffer, float size_of_vertex);
	void clearRT();
	void mapCBs();
	void layoutTopology(int in_topology, int in_layout);
	//void setDrawCall(int nr_verticies, int drawType);
	void setQuad();
public:

	RenderEngine(HWND wndHandle, HINSTANCE hInstance, int WIDTH, int HEIGHT);
	~RenderEngine();

	ObjLoader * getLoader();
	void update();
	void Draw(Terrain * in_terrain); // draw called object
	void Draw(Geometry * in_geometry);
	void Draw(Box* in_box);
	ID3D11Device* getDevice();
	void addSRV(ID3D11ShaderResourceView * in_srv);
	void clearRenderTargets();
	void presentSC();
	void lightPass();
	void geometryPass(int nr_verticies, int drawType);
};

#endif