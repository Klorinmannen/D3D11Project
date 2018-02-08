#include"RenderEngine.h"
#include "windows.h"

const int HEIGHT = 1080;
const int WIDTH = 1920;

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void terrainPreSet(Terrain &terrain, RenderEngine &Engine);




int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);

	RenderEngine Engine(wndHandle, hInstance, WIDTH, HEIGHT);
	
	//Heightmap
	Terrain myTerrain(Engine.getDevice());
	terrainPreSet(myTerrain, Engine);

	//deferred Quad
	


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
				Engine.update();
				Engine.Draw(&myTerrain);
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

void terrainPreSet(Terrain &terrain, RenderEngine &Engine)
{
	terrain.createBuffers();
	Engine.setMatrixes(terrain.getWorldMatrix());
}
