#include"RenderEngine.h"
#include "windows.h"
#include<algorithm>
#include <stdlib.h>  
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC

const int WIDTH = 1024;
const int HEIGHT = 1024;


HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool compare(const Drawable* lhs, const Drawable* rhs);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);

	RenderEngine Engine(wndHandle, hInstance, WIDTH, HEIGHT);
	std::vector<Drawable*> objects;

	//Heightmap
	Box myBox;
	myBox.setLoader(Engine.getLoader());
	myBox.setDevice(Engine.getDevice());
	myBox.initiate();
	Engine.addBoxSRV(myBox.getSRV());
	myBox.distanceToCam(Engine.getCamPos());
	objects.push_back(&myBox);

	Box myBox2;
	myBox2.setLoader(Engine.getLoader());
	myBox2.setDevice(Engine.getDevice());
	myBox2.initiate();
	myBox2.setTranslationMatix(XMMatrixTranslation(4000.0f, 3400.0f, -11000.0f));
	myBox2.setScaleMatrix(XMMatrixScaling(20, 20, 20));
	myBox2.distanceToCam(Engine.getCamPos());
	objects.push_back(&myBox2);

	Box myBox3;
	myBox3.setLoader(Engine.getLoader());
	myBox3.setDevice(Engine.getDevice());
	myBox3.initiate();
	myBox3.setTranslationMatix(XMMatrixTranslation(6000.0f, 1400.0f, -6000.0f));
	myBox3.setScaleMatrix(XMMatrixScaling(20, 20, 20));
	myBox3.distanceToCam(Engine.getCamPos());
	objects.push_back(&myBox3);


	Box myBox4;
	myBox4.setLoader(Engine.getLoader());
	myBox4.setDevice(Engine.getDevice());
	myBox4.initiate();
	myBox4.setTranslationMatix(XMMatrixTranslation(9000.0f, 6000.0f, -10000.0f));
	myBox4.setScaleMatrix(XMMatrixScaling(20, 20, 20));
	myBox4.distanceToCam(Engine.getCamPos());
	objects.push_back(&myBox4);


	Terrain myTerrain(Engine.getDevice());
	myTerrain.createBuffers();
	myTerrain.createTexture();
	myTerrain.createSamplerState();
	Engine.addTerrainSRV(myTerrain.getSRV(), myTerrain.getNrOfSRVS());

	
	Plane myPlane;
	myPlane.setDevice(Engine.getDevice());
	myPlane.initiate();
	myPlane.distanceToCam(Engine.getCamPos());
	objects.push_back(&myPlane);

	Plane myPlane2;
	myPlane2.setDevice(Engine.getDevice());
	myPlane2.initiate();

	Plane::Vertex vertices[4];
	vertices[0].pos = XMFLOAT3(-1.0f, 0.0f, 1.0f);
	vertices[0].uvs = XMFLOAT2(0.0f, 0.0f);

	vertices[1].pos = XMFLOAT3(1.0f, 0.0f, 1.0f);
	vertices[1].uvs = XMFLOAT2(1.0f, 0.0f);

	vertices[2].pos = XMFLOAT3(1.0f, 0.0f, -1.0f);
	vertices[2].uvs = XMFLOAT2(1.0f, 1.0f);

	vertices[3].pos = XMFLOAT3(-1.0f, 0.0f, -1.0f);
	vertices[3].uvs = XMFLOAT2(0.0f, 1.0f);

	int index[6];
	index[0] = 0;
	index[1] = 1;
	index[2] = 3;
	index[3] = 3;
	index[4] = 1;
	index[5] = 2;

	myPlane2.setVerticesAndIndex(vertices, index);
	myPlane2.setTranslationMatix(XMMatrixTranslation(6000.0f, 6000.0f, -16000.0f));
	myPlane2.setScaleMatrix(XMMatrixScaling(6000.0f, 1.0f, 6000.0f));
	myPlane2.setRotationMatrix(XMMatrixRotationX(XM_PIDIV2));
	myPlane2.distanceToCam(Engine.getCamPos());
	objects.push_back(&myPlane2);

	Catnip myCat(HEIGHT, WIDTH);
	myCat.setDevice(Engine.getDevice());
	myCat.initiate();

	Catnip::vertex verticess[4];
	verticess[0].pos = XMFLOAT3(-1.0f, 0.0f, 1.0f);
	verticess[0].uvs = XMFLOAT2(0.0f, 0.0f);

	verticess[1].pos = XMFLOAT3(1.0f, 0.0f, 1.0f);
	verticess[1].uvs = XMFLOAT2(1.0f, 0.0f);

	verticess[2].pos = XMFLOAT3(1.0f, 0.0f, -1.0f);
	verticess[2].uvs = XMFLOAT2(1.0f, 1.0f);

	verticess[3].pos = XMFLOAT3(-1.0f, 0.0f, -1.0f);
	verticess[3].uvs = XMFLOAT2(0.0f, 1.0f);
	
	myCat.setIndexAndVertices(index, verticess);
	myCat.createBuffers();
	myCat.SetTranslationMatix(XMMatrixTranslation(10000.0f, 2000.0f, -2000.0f));
	myCat.setScaleMatrix(XMMatrixScaling(1000.0f, 1.0f, 1000.0f));
	myCat.setRotationMatrix(XMMatrixRotationX(XM_PIDIV2) * XMMatrixRotationZ(XM_PI));
	myCat.distanceToCam(Engine.getCamPos());
	//objects.push_back(&myCat);


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

				for (auto var : objects)
				{
					var->distanceToCam(Engine.getCamPos());
				}

				Engine.update();
				//sort objects to render in order front to back
				std::sort(objects.begin(), objects.end(), &compare);

				//clear renderTargets
				Engine.clearRenderTargets();

				//draw scene

				for (auto var : objects)
				{
					Engine.Draw(var);
				}
				
				Engine.Draw(&myTerrain);
				Engine.Draw(&myCat);
				//Engine.Draw(&myTerrain);
				//Engine.Draw(&myBox);
				//Engine.Draw(&myPlane);
				//Engine.Draw(&myPlane2);
				//Engine.Draw(&myBox2);
				//Engine.Draw(&myBox3);
				//Engine.Draw(&myBox4);

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

bool compare(const Drawable * lhs, const Drawable * rhs)
{
	//predency for std::sort()
	return lhs->getDistance() < rhs->getDistance();
}
