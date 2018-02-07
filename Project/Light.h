#ifndef LIGHT_H
#define LIGHT_H


#include"IncludeDX11.h"

class Light
{

private:
	struct lights
	{
		XMVECTOR position; //w is 1 for points
		XMVECTOR color; //w is color intensity
	};

	lights light1;

	void initateLights();

public:
	Light();
	~Light();
	lights getLights();
};
#endif // !LIGHT_H