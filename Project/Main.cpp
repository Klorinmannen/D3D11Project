#include"RenderEngine.h"
#include "windows.h"
#include"ObjLoader.h"

const int HEIGHT = 800;
const int WIDTH = 800;

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void terrainPreSet(Terrain &terrain, RenderEngine &Engine);
void boxPreSet(Box &box, RenderEngine &Engine);



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
	Engine.addSRV(myBox.getSRV());
	Terrain myTerrain(Engine.getDevice());
	myTerrain.createBuffers();



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
