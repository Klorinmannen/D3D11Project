#ifndef PLANE_H
#define PLANE_H
#include"IncludeDX11.h"
#include<vector>
#include"Settings.h"

class Plane
{
public:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uvs;
	};

private:
	Vertex vertices[4];
	int index[6];
	int topology;
	int layout;

	XMMATRIX world = XMMatrixScaling(6000.0f, 1.0f, 6000.0f) * XMMatrixTranslation(6000.0f, 0.0f, -10000.0f);

	ID3D11Device * device;
	ID3D11Buffer * vBuffer;
	ID3D11Buffer * iBuffer;
	ID3D11SamplerState * sampState;
	void createVertices();
	void createBuffers();
	void createSamplerState();
public:
	Plane();
	~Plane();
	XMMATRIX getWorldMatrix();
	ID3D11Buffer * getVertexBuffer();
	ID3D11Buffer * getIndexBuffer();
	ID3D11SamplerState * getSamplerState();
	void setDevice(ID3D11Device * in_device);
	void initiate();

	void setVerticesAndIndex(Vertex * in_vertices, int * in_index);
	void setWorldMatrix(XMMATRIX in_world);

	int getTopology();
	int getLayout();
	int getSizeOfVertex();
	int getNrOfVertices();

};
#endif // !PLANE_H

