#include "Geometry.h"

void Geometry::setVertexArray(Structs::PC_Vertex * in_array, int nr_vertex)
{
	this->vertices = in_array;
	this->vertex_number = nr_vertex;
}

void Geometry::setIndexArray(int * in_array)
{
	this->indexArray = in_array;
}

Geometry::Geometry(ID3D11Device * device) : Drawable()
{
	this->device = device;
}

Geometry::~Geometry()
{
}

bool Geometry::createBuffers()
{
	D3D11_BUFFER_DESC buffer_desc;
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	buffer_desc.ByteWidth = sizeof(Structs::PC_Vertex);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = this->vertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hResult = this->device->CreateBuffer(&buffer_desc, &data, &this->vertexBuffer);
	if (FAILED(hResult))
	{
		return false;
	}

	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	buffer_desc.ByteWidth = sizeof(this->indexArray);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	ZeroMemory(&data, sizeof(data));
	data.pSysMem = this->indexArray;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hResult = this->device->CreateBuffer(&buffer_desc, &data, &this->indexBuffer);
	
	if (FAILED(hResult))
	{
		return false;
	}
	return true;
}

ID3D11Buffer * Geometry::getVertexBuffer()
{
	return this->vertexBuffer;
}

ID3D11Buffer * Geometry::getIndexBuffer()
{
	return this->indexBuffer;
}

float Geometry::getSizeOfVertex()
{
	return sizeof(Structs::PC_Vertex);
}

int Geometry::getTopology()
{
	return this->t_topology;
}

int Geometry::getLayout()
{
	return this->t_layout;
}

int Geometry::getNrOfVertices()
{
	return this->vertex_number;
}

XMMATRIX Geometry::getWorld() const
{
	return  DirectX::XMMatrixScaling(1, 1, 1) *  DirectX::XMMatrixTranslation(-1500.0f, -1500.0f, -1500.0f);
}
