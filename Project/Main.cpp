#include "GeometryHandler.h"
#include "windows.h"

const int HEIGHT = 1280;
const int WIDTH = 720;

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);

	RenderEngine Engine(wndHandle, WIDTH, HEIGHT);
	GeometryHandler handler(&Engine);
	

	if (wndHandle)
	{
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				handler.update();
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
	wcex.lpszClassName = "BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
	{
		DWORD dw = GetLastError();
		return false;
	}
	RECT rc = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
								"BTH_D3D_DEMO",
								"BTH Direct3D Demo",
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