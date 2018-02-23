#include"RenderEngine.h"
#include "windows.h"
#include"ObjLoader.h"

const int WIDTH = 1000;
const int HEIGHT = 1000;


HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);




int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);

	RenderEngine Engine(wndHandle, hInstance, WIDTH, HEIGHT);
	
	//Heightmap
	Box myBox;
	myBox.setLoader(Engine.getLoader());
	myBox.setDevice(Engine.getDevice());
	myBox.initiate();
	Engine.addBoxSRV(myBox.getSRV());

	Box myBox2;
	myBox2.setLoader(Engine.getLoader());
	myBox2.setDevice(Engine.getDevice());
	myBox2.initiate();
	myBox2.setWorldMatrix(XMMatrixScaling(20, 20, 20) * XMMatrixTranslation(6000.0f, 3400.0f, -12000.0f));
	Engine.addBoxSRV(myBox2.getSRV());

	Terrain myTerrain(Engine.getDevice());
	myTerrain.createBuffers();
	myTerrain.createTexture();
	myTerrain.createSamplerState();
	Engine.addTerrainSRV(myTerrain.getSRV(), myTerrain.getNrOfSRVS());

	
	Plane myPlane;
	myPlane.setDevice(Engine.getDevice());
	myPlane.initiate();

	Plane myPlane2;
	myPlane2.setDevice(Engine.getDevice());
	myPlane2.initiate();



	Plane::Vertex vertices[4];
	vertices->pos = XMFLOAT3(-1.0f, 0.0f, 1.0f);
	vertices->uvs = XMFLOAT2(0.0f, 0.0f);

	vertices->pos = XMFLOAT3(1.0f, 0.0f, 1.0f);
	vertices->uvs = XMFLOAT2(1.0f, 0.0f);

	vertices->pos = XMFLOAT3(1.0f, 0.0f, -1.0f);
	vertices->uvs = XMFLOAT2(1.0f, 1.0f);

	vertices->pos = XMFLOAT3(-1.0f, 0.0f, -1.0f);
	vertices->uvs = XMFLOAT2(0.0f, 1.0f);

	int index[6];
	index[0] = 0;
	index[1] = 1;
	index[2] = 3;
	index[3] = 3;
	index[4] = 1;
	index[5] = 2;

	myPlane2.setVerticesAndIndex(vertices, index);
	myPlane2.setWorldMatrix(XMMatrixScaling(6000.0f, 1.0f, 6000.0f) * XMMatrixTranslation(6000.0f, -16000.0f, -6000.0f) * XMMatrixRotationX(XM_PIDIV2));


	if (wndHandle)
	{
		ShowWindow(wndHandle, nCmdShow);
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				//clear renderTargets
				Engine.clearRenderTargets();

				//draw scene
				Engine.update();
				Engine.Draw(&myTerrain);
				Engine.Draw(&myBox);
				Engine.Draw(&myPlane);
				Engine.Draw(&myPlane2);
				Engine.Draw(&myBox2);
				//draw fullscreen quad and lights
				Engine.lightPass();

				//update swapChain
				Engine.presentSC();
			}

		}
	}



}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = "D3D11 Projekt";
	if (!RegisterClassEx(&wcex))
	{
		DWORD dw = GetLastError();
		return false;
	}
	RECT rc = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
								"D3D11 Projekt",
								"D3D Projekt Fönster",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								rc.right - rc.left,
								rc.bottom - rc.top,
								nullptr,
								nullptr,
								hInstance,
								nullptr);


	if (FAILED(handle))
	{
		DWORD dw = GetLastError();
		return false;
	}
	return handle;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
