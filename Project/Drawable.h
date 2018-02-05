#ifndef DRAWABLE_H
#define DRAWABLE_H


#include"RenderEngine.h"
#include "Terrain.h"
#include"Settings.h"
#include"IncludeDX11.h"
/*
Interface för objekt som ska kunnas ritas ut
*/

class Drawable
{
public:
	Drawable(RenderEngine* engine);
	~Drawable();

protected:
	ID3D11Device * device;
	RenderEngine * renderEngine;

	virtual void Draw() = 0;
	virtual ID3D11Buffer* getVertexBuffer() = 0;
	virtual ID3D11Buffer* getIndexBuffer() = 0;
	virtual float getSizeOfVertex() = 0;
	virtual topology getTopology() = 0;
	virtual layout getLayout() = 0;
	virtual int getNrOfVertices() = 0;
};

#endif // !DRAWABLE_H