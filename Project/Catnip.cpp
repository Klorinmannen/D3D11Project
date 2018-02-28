#include "Catnip.h"
Catnip::Catnip(int in_height, int in_width)
{
	this->rotation = XMMatrixIdentity();
	this->translation = XMMatrixIdentity();
	this->scale = XMMatrixIdentity();

	this->width = in_width;
	this->height = in_height;

	this->device = nullptr;
	this->catSRV = nullptr;
	this->vBuffer = nullptr;
	this->iBuffer = nullptr;

	for (int i = 0; i < ARRAYSIZE(this->vertices); i++)
	{
		this->vertices[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->vertices[i].uvs = XMFLOAT2(0.0f, 0.0f);
	}
	for (int i = 0; i < ARRAYSIZE(this->index); i++)
	{
		this->index[i] = -1;
	}

	this->topology = topology::TriangleList;
	this->layout = layout::PT;

}

Catnip::~Catnip()
{
	this->catSRV->Release();
	this->vBuffer->Release();
	this->iBuffer->Release();
}

void Catnip::loadWICTexture()
{
	HRESULT hr;
	hr = CreateWICTextureFromFile(this->device, L"cat.jpg", nullptr, &this->catSRV);
	if (FAILED(hr))
	{
		exit(-1);
	}
}

void Catnip::createBuffers()
{
	D3D11_BUFFER_DESC catDesc;
	D3D11_SUBRESOURCE_DATA catData;

	//Resett descriptions
	ZeroMemory(&catDesc, sizeof(catDesc));

	//Flags
	catDesc.Usage = D3D11_USAGE_DEFAULT;
	catDesc.ByteWidth = sizeof(this->index);
	catDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	catDesc.CPUAccessFlags = 0;
	catDesc.MiscFlags = 0;

	catData.pSysMem = this->index;

	this->device->CreateBuffer(&catDesc, &catData, &this->iBuffer);

	//Resett description
	ZeroMemory(&catDesc, sizeof(catDesc));

	//Flags
	catDesc.Usage = D3D11_USAGE_DEFAULT;
	catDesc.ByteWidth = sizeof(this->vertices);
	catDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	catDesc.CPUAccessFlags = 0;
	catDesc.MiscFlags = 0;

	catData.pSysMem = this->vertices;

	this->device->CreateBuffer(&catDesc, &catData, &this->vBuffer);


}

XMMATRIX Catnip::getWorldMatrix()
{
	return this->scale * this->rotation * this->translation;
}

void Catnip::SetTranslationMatix(XMMATRIX in_trans)
{
	this->translation = in_trans;
}

void Catnip::setScaleMatrix(XMMATRIX in_scale)
{
	this->scale = in_scale;
}

void Catnip::setRotationMatrix(XMMATRIX in_rotation)
{
	this->rotation = in_rotation;
}

int Catnip::getLayout()
{
	return this->layout;
}

int Catnip::getTopology()
{
	return this->topology;
}

int Catnip::getNrOfVertices()
{
	return ARRAYSIZE(this->index);
}

int Catnip::getSizeOfVertex()
{
	return sizeof(vertex);
}

void Catnip::initiate()
{
	this->loadWICTexture();
}

void Catnip::setDevice(ID3D11Device * in_device)
{
	this->device = in_device;
}

void Catnip::setIndexAndVertices(int * in_index, vertex * in_vertices)
{
	this->vertices[0] = in_vertices[0];
	this->vertices[1] = in_vertices[1];
	this->vertices[2] = in_vertices[2];
	this->vertices[3] = in_vertices[3];

	this->index[0] = in_index[0];
	this->index[1] = in_index[1];
	this->index[2] = in_index[2];
	this->index[3] = in_index[3];
	this->index[4] = in_index[4];
	this->index[5] = in_index[5];
}

ID3D11Buffer * Catnip::getVertexBuffer()
{
	return this->vBuffer;
}

ID3D11Buffer * Catnip::getIndexBuffer()
{
	return this->iBuffer;
}

ID3D11ShaderResourceView * Catnip::getCatSRV() const
{
	return this->catSRV;
}
