#ifndef LIGHT_H
#define LIGHT_H

#include"ObjLoader.h"
#include"IncludeDX11.h"

class Light
{

private:
	struct lights
	{
		XMFLOAT4 direction; //w is 1 for points
		XMFLOAT4 color; //w is color intensity
		XMFLOAT4 camPos;
		XMFLOAT4 lightPos;
	};

	lights light1;
	ObjLoader loader;
	ID3D11Device * device;

	void initateLights();

public:
	Light();
	~Light();
	lights getLights();
	void setLightPos(XMFLOAT4 in_pos);
	void setCamPos(XMFLOAT4 new_pos);
};
#endif // !LIGHT_H