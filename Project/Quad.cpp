#include "Quad.h"

void Quad::createBuffers()
{
	this->createIndexBuffer();
	this->createVertexBuffer();
}

void Quad::createIndexBuffer()
{
	//ID3DBuffer Index Buffer
	//iBuffer

	D3D11_SUBRESOURCE_DATA iData;
	D3D11_BUFFER_DESC iDesc;

	//Resett descriptions
	ZeroMemory(&iDesc, sizeof(iDesc));
	ZeroMemory(&iData, sizeof(iData));

	//Flags
	iDesc.Usage = D3D11_USAGE_DEFAULT;
	iDesc.ByteWidth = sizeof(this->indexArray);
	iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iDesc.CPUAccessFlags = 0;
	iDesc.MiscFlags = 0;

	iData.pSysMem = this->indexArray;

	this->q_device->CreateBuffer(&iDesc, &iData, &this->iBuffer);
}

void Quad::createVertexBuffer()
{
	//ID3DBuffer Vertex Buffer
	//vBuffer

	D3D11_SUBRESOURCE_DATA vData;
	D3D11_BUFFER_DESC vDesc;

	//Resett descriptions
	ZeroMemory(&vDesc, sizeof(vDesc));
	ZeroMemory(&vData, sizeof(vData));

	//Flags
	vDesc.Usage = D3D11_USAGE_DEFAULT;
	vDesc.ByteWidth = sizeof(this->vertices);
	vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vDesc.CPUAccessFlags = 0;
	vDesc.MiscFlags = 0;

	vData.pSysMem = this->vertices;

	this->q_device->CreateBuffer(&vDesc, &vData, &this->vBuffer);

}

Quad::Quad()
{
	//top left 0
	this->vertices[0].pos = { -1.0f, 1.0f, 0.0f };
	//top right 1
	this->vertices[1].pos = { 1.0f, 1.0f, 0.0f };
	//bottom left 2
	this->vertices[2].pos = { -1.0f, -1.0f, 0.0f };
	//bottom right 3
	this->vertices[3].pos = { 1.0f, -1.0f, 0.0f };


	//First triangle clockwise winding
	this->indexArray[0] = 0;
	this->indexArray[1] = 1;
	this->indexArray[2] = 2;

	//Second triangle clockwise winding
	this->indexArray[3] = 1;
	this->indexArray[4] = 3;
	this->indexArray[5] = 2;

	//used settings
	this->q_topology = topology::TriangleList;
	this->q_layout = layout::Pos;

}

Quad::~Quad()
{

}

Quad::vertex* Quad::getVertices()
{
	return this->vertices;
}

int * Quad::getIndex()
{
	return this->indexArray;
}

float Quad::getSizeOfVertex()
{
	return sizeof(vertex);
}

ID3D11Buffer * Quad::getVertexBuffer()
{
	return this->vBuffer;
}

ID3D11Buffer * Quad::getIndexBuffer()
{
	return this->iBuffer;
}

int Quad::getTopology()
{
	return this->q_topology;
}

int Quad::getLayout()
{
	return this->q_layout;
}

int Quad::getNrOfVertices()
{
	return ARRAYSIZE(this->indexArray);
}

void Quad::setDevice(ID3D11Device * in_device)
{
	this->q_device = in_device;
}
