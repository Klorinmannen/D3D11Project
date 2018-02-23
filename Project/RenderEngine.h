#ifndef RenderEngine_H
#define RenderEngine_H

#include"ShadowMap.h"
#include"Plane.h"
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
	enum pass { Geometry_pass, Lightning_pass };

	bool useRastBackCull = true;
	int HEIGHT;
	int WIDTH;
	float nearZ = 0.1f;
	float farZ = 50000.0f;
	const int RTV_VIEW_COUNT = 3;
	const int SRV_VIEW_COUNT = 4;

	//book example
	struct BoundingSphere
	{
		XMFLOAT3 centre;
		float radius;
	};
	BoundingSphere bSphere;

	XMVECTOR lightPos;
	XMVECTOR target;
	XMVECTOR upVec;
	XMVECTOR lightDir;

	//Transformation matrixes
	struct matrix_wvp
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX inverseWorld;
		XMMATRIX wvp;
		XMMATRIX vpLight;
	};
	matrix_wvp m_wvp;


	Quad quad;
	ObjLoader objLoader;
	Camera camera;
	DeferredShaders deferred_shading;
	Light lights;
	ShadowMap sMap;

	float black[4];

private:
	//D3D11 data

	IDXGISwapChain * swapChain;
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;

	//RT Krakens
	std::vector<ID3D11RenderTargetView*> RTViews;
	std::vector<ID3D11ShaderResourceView*> SRViews;
	ID3D11ShaderResourceView* null4[4] = { nullptr, nullptr, nullptr, nullptr };
	ID3D11ShaderResourceView* null2[2] = { nullptr, nullptr };
	ID3D11ShaderResourceView* null1[1] = { nullptr };
	ID3D11Buffer * nullBuffer = nullptr;
	ID3D11ShaderResourceView * sampBoxTexture;
	std::vector<ID3D11ShaderResourceView*> sampTerrainTextures;
	ID3D11SamplerState* tempState;

	//backbuffer

	ID3D11RenderTargetView * back_buffer_view;

	ID3D11RasterizerState * RSState;
	ID3D11DepthStencilState * DSState;

	ID3D11DepthStencilView * depthStencilView; //1 is shadowview
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
	void setShadowStuff();


	//Render functions
	void updateMatrixes(const XMMATRIX &in_world);
	void updateShaders(ID3D11VertexShader * in_vertexS, ID3D11GeometryShader * in_geometryS, ID3D11PixelShader * in_pixelS);
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
	void Draw(Box* in_box);
	void Draw(Plane * in_plane);
	ID3D11Device* getDevice();
	void addBoxSRV(ID3D11ShaderResourceView * in_srv);
	void addTerrainSRV(ID3D11ShaderResourceView ** in_srv, int size);
	void clearRenderTargets();
	void presentSC();
	void lightPass();
	void shadowPass(int nr_verticies, int drawType);
	void geometryPass(int nr_verticies, int drawType);
};

#endif