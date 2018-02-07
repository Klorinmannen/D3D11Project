#ifndef QUAD_H
#define QUAD_H
#include"Settings.h"
#include"Drawable.h"

class Quad : public Drawable
{
private:

	struct vertex
	{
		XMFLOAT3 pos;
	};

	vertex vertices[4];
	int indexArray[6];

	ID3D11Buffer * iBuffer;
	ID3D11Buffer * vBuffer;

	ID3D11Device * q_device;

	int q_topology;
	int q_layout;


	void createIndexBuffer();
	void createVertexBuffer();

public:
	Quad();
	~Quad();

	vertex* getVertices();
	int * getIndex();
	float getSizeOfVertex();
	ID3D11Buffer * getVertexBuffer();
	ID3D11Buffer * getIndexBuffer();
	int getTopology();
	int getLayout();
	int getNrOfVertices();

	void createBuffers();
	void setDevice(ID3D11Device * in_device);


};

#endif // !QUAD_H
