#include "Box.h"
Box::Box() : Drawable()
{
	this->vertices = nullptr;
	this->normals = nullptr;
	this->uvs = nullptr;
	this->index = nullptr;

	this->objName = "box.obj";
	this->picName = L"boxP.png";
	this->topology = topology::TriangleList;
	this->layout = layout::PTN;

	this->m_scale = XMMatrixScaling(20, 20, 20);
	this->m_translation = XMMatrixTranslation(5400.0f, 3400.0f, 6400.0f);
	this->m_world = this->m_scale * this->m_translation;
}

Box::~Box()
{
	delete this->vertices;
	delete this->normals;
	delete this->uvs;
	delete this->index;

	this->vBuffer->Release();
	this->srv->Release();
}

void Box::buildVertexVec()
{
	//assemble vertex
	
	if (this->normals->size() > 0)
	{
		for (int i = 0; i < this->vertices->size(); i++)
		{
			vertex.v_pos = (*this->vertices)[i];
			vertex.v_normal = (*this->normals)[i];
			vertex.uvs = (*this->uvs)[i];



			this->vertexVec.push_back(this->vertex);
		}
	}
	else
	{
		for (int i = 0; i < this->vertices->size(); i++)
		{
			vertex.v_pos = (*this->vertices)[i];
			//vertex.v_normal = (*this->normals)[i];
			vertex.uvs.x = (*this->uvs)[i].x;
			vertex.uvs.y = (*this->uvs)[i].y;

			this->vertexVec.push_back(this->vertex);
		}
	}

	//can index .. 
	std::string indexing = "";

}

void Box::load()
{
	//call to load the specified .obj file and its texture
	this->loader->loadObjFile(this->objName, this->picName, this->vertices, this->normals, this->uvs, this->srv);

}

void Box::createBuffers()
{

	//create vertexbuffer
	HRESULT hr;

	D3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA data;

	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&data, sizeof(data));

	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.ByteWidth = sizeof(PTN_Vertex) * this->vertexVec.size();
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;

	data.pSysMem = this->vertexVec.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = this->device->CreateBuffer(&bDesc, &data, &this->vBuffer);
}

void Box::findMidPoint()
{
	XMFLOAT3 first = this->vertexVec[0].v_pos;
	XMFLOAT3 second;

	//find third point with a different z, a box has only two different z values.
	for (auto var : this->vertexVec)
	{
		if (var.v_pos.z != first.z && var.v_pos.x != first.x && var.v_pos.y != first.y)
		{
			second = var.v_pos;
			break;
		}
	}

	//find deltas
	float deltaX = (first.x + second.x) * 0.5;
	float deltaY = (first.y + second.y) * 0.5;
	float deltaZ = (first.z + second.z) * 0.5;

	//final midpoint in model-coordinates
	this->midPoint = XMFLOAT4(deltaX, deltaY, deltaZ, 1.0f);

}

void Box::transformMidPoint()
{
	//transform midpoint vector to world coordinates
	XMVECTOR temp = XMLoadFloat4(&this->midPoint);
	//Transform
	temp = XMVector3Transform(temp, this->m_translation);
	XMStoreFloat4(&this->midPoint, temp);
}

void Box::update()
{
	this->m_world = this->m_scale * this->m_translation;
}

void Box::initiate()
{
	this->load();
	this->buildVertexVec();
	this->createBuffers();
	this->findMidPoint();
	this->transformMidPoint();
}

void Box::setLoader(ObjLoader * in_loader)
{

	this->loader = in_loader;
}

void Box::setDevice(ID3D11Device * in_device)
{
	this->device = in_device;
}

ID3D11Buffer * Box::getVertexBuffer()
{
	return this->vBuffer;
}

ID3D11Buffer * Box::getIndexBuffer()
{
	//not used as for now
	return nullptr;
}

float Box::getSizeOfVertex()
{
	return sizeof(PTN_Vertex);
}

int Box::getTopology()
{
	return this->topology;
}

int Box::getLayout()
{
	return this->layout;
}

int Box::getNrOfVertices()
{
	return this->vertexVec.size();
}

void Box::setTranslationMatix(XMMATRIX in_trans)
{
	this->m_translation = in_trans;
	this->transformMidPoint();
}

void Box::setScaleMatrix(XMMATRIX in_scale)
{
	this->m_scale = in_scale;
	this->update();
}

ID3D11ShaderResourceView * Box::getSRV()
{
	return this->srv;
}

bool Box::operator<(const Box & other) const
{
	return this->distance < other.distance;
}

XMMATRIX Box::getWorldMatrix() const
{
	return m_world;
}
