#ifndef DRAWABLE_H
#define DRAWABLE_H
#include"IncludeDX11.h"
#include "Settings.h"
/*
Interface för objekt som ska kunnas ritas ut
*/

class Drawable
{
public:
	Drawable();
	~Drawable();

protected:
	virtual ID3D11Buffer* getVertexBuffer() = 0;
	virtual ID3D11Buffer* getIndexBuffer() = 0;
	virtual float getSizeOfVertex() = 0;
	virtual int getTopology() = 0;
	virtual int getLayout() = 0;
	virtual int getNrOfVertices() = 0;
};

#endif // !DRAWABLE_H