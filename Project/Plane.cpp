#include "Plane.h"



Plane::Plane() : Drawable()
{
	this->topology = topology::TriangleList;
	this->layout = layout::PT;
	this->m_scale = XMMatrixScaling(6000.0f, 1.0f, 6000.0f);
	this->m_translation = XMMatrixTranslation(6000.0f, 0.0f, -10000.0f);
	this->m_rotation = XMMatrixIdentity();
	this->m_world = this->m_scale * this->m_translation;
	this->device = nullptr;
	this->vBuffer = nullptr;
	this->iBuffer = nullptr;
	this->sampState = nullptr;
}

Plane::~Plane()
{
	this->vBuffer->Release();
	this->iBuffer->Release();
	this->sampState->Release();
}

void Plane::createVertices()
{
	this->vertices[0].pos = XMFLOAT3(1.0f, 0, -1.0f); //front left
	this->vertices[0].uvs = XMFLOAT2(0, 0); //uvs
	this->vertices[1].pos = XMFLOAT3(-1.0f, 0, -1.0f); //front right
	this->vertices[1].uvs = XMFLOAT2(1, 0);
	this->vertices[2].pos = XMFLOAT3(1.0f, 0, 1.0f); //back left
	this->vertices[2].uvs = XMFLOAT2(0, 1);
	this->vertices[3].pos = XMFLOAT3(-1.0f, 0, 1.0f); //back right
	this->vertices[3].uvs = XMFLOAT2(1, 1);

	this->index[0] = 0;
	this->index[1] = 1;
	this->index[2] = 3;
	this->index[3] = 0;
	this->index[4] = 3;
	this->index[5] = 2;
}

void Plane::createBuffers()
{
	HRESULT hr;
	D3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA data;

	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&data, sizeof(data));

	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.ByteWidth = sizeof(this->vertices);
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;

	data.pSysMem = this->vertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = this->device->CreateBuffer(&bDesc, &data, &this->vBuffer);

	D3D11_BUFFER_DESC iDesc;
	D3D11_SUBRESOURCE_DATA idata;

	ZeroMemory(&iDesc, sizeof(iDesc));
	ZeroMemory(&idata, sizeof(idata));

	iDesc.Usage = D3D11_USAGE_DEFAULT;
	iDesc.ByteWidth = sizeof(this->index);
	iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iDesc.CPUAccessFlags = 0;
	iDesc.MiscFlags = 0;

	idata.pSysMem = this->index;
	idata.SysMemPitch = 0;
	idata.SysMemSlicePitch = 0;

	hr = this->device->CreateBuffer(&iDesc, &idata, &this->iBuffer);
}

void Plane::createSamplerState()
{
	//Create Desc
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	//Flags
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create samplerState
	device->CreateSamplerState(&sampDesc, &this->sampState);
}

void Plane::findMidPoint()
{
	XMFLOAT3 first = this->vertices[0].pos;
	XMFLOAT3 second;

	//fint opposite point
	for (auto var : this->vertices)
	{
		if (first.x != var.pos.x && first.z != var.pos.z)
		{
			second = var.pos;
			break;
		}
	}

	float deltaX = (first.x + second.x) * 0.5;
	float deltaZ = (first.z + second.z) * 0.5;
	this->midPoint = XMFLOAT4(deltaX, first.y, deltaZ, 1.0f);
	if (deltaX == 0.0f && deltaZ == 0.0f)
	{
		this->midPoint = XMFLOAT4(1, 1, 1, 1);
	}


}

XMMATRIX Plane::getWorldMatrix()
{
	return this->m_world;
}

ID3D11Buffer * Plane::getVertexBuffer()
{
	return this->vBuffer;
}

ID3D11Buffer * Plane::getIndexBuffer()
{
	return this->iBuffer;
}

ID3D11SamplerState * Plane::getSamplerState()
{
	return this->sampState;
}

void Plane::setDevice(ID3D11Device * in_device)
{
	this->device = in_device;
}

void Plane::initiate()
{
	this->createVertices();
	this->createBuffers();
	this->createSamplerState();
	this->findMidPoint();
}

void Plane::setVerticesAndIndex(Vertex * in_vertices, int * in_index)
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

	this->findMidPoint();
}

void Plane::setRotationMatrix(XMMATRIX in_rot)
{
	this->m_rotation = in_rot;
	this->update();
}

void Plane::setTranslationMatix(XMMATRIX in_trans)
{
	this->m_translation = in_trans;
	this->transformMidPoint();
}

void Plane::setScaleMatrix(XMMATRIX in_scale)
{
	this->m_scale = in_scale;
	this->update();
}

void Plane::transformMidPoint()
{
	//transform midpoint vector to world coordinates
	XMVECTOR temp = XMLoadFloat4(&this->midPoint);
	//coordinate transformation ignore .w
	temp = XMVector3Transform(temp, this->m_rotation * this->m_translation);
	XMStoreFloat4(&this->midPoint, temp);
}

void Plane::update()
{
	this->m_world = this->m_scale * this->m_rotation * this->m_translation;

}

int Plane::getTopology()
{
	return this->topology;
}

int Plane::getLayout()
{
	return this->layout;
}

int Plane::getSizeOfVertex()
{
	return sizeof(Vertex);
}

int Plane::getNrOfVertices()
{
	return ARRAYSIZE(this->index);
}

bool Plane::operator<(const Plane & other) const
{
	return this->distance < other.distance;
}
