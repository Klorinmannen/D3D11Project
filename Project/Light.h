#ifndef LIGHT_H
#define LIGHT_H


#include"IncludeDX11.h"

class Light
{

private:
	struct light
	{
		XMVECTOR position; //w is 1 for points
		XMVECTOR color; //w is color intensity
	};

	light lights[2];

	void initateLights();

public:
	Light();
	~Light();
	light c_light;
	light* getLights();
};
#endif // !LIGHT_H