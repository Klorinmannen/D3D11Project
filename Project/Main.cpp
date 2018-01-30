#include"GeometryHandler.h"



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

	RenderEngine Engine(hInstance, nCmdShow);
	GeometryHandler handler(&Engine);

}