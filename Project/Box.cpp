#include "Box.h"
Box::Box()
{
	this->vertices = nullptr;
	this->normals = nullptr;
	this->uvs = nullptr;
	this->index = nullptr;

	this->objName = "box.obj";
	this->picName = L"boxP.png";
	this->id = -1;
	this->topology = topology::TriangleList;
	this->layout = layout::PTN;
}

Box::~Box()
{
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

	//D3D11_BUFFER_DESC iDesc;
	//D3D11_SUBRESOURCE_DATA idata;

	//ZeroMemory(&iDesc, sizeof(iDesc));
	//ZeroMemory(&idata, sizeof(idata));

	//iDesc.Usage = D3D11_USAGE_DEFAULT;
	//iDesc.ByteWidth = sizeof(int) * this->index->size();
	//iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//iDesc.CPUAccessFlags = 0;
	//iDesc.MiscFlags = 0;

	//idata.pSysMem = this->index->data();
	//idata.SysMemPitch = 0;
	//idata.SysMemSlicePitch = 0;

	//hr = this->device->CreateBuffer(&iDesc, &idata, &this->iBuffer);
}

void Box::indexThis()
{
	this->vertices; //24st
	this->normals; //24st
	this->uvs; //24st;



}

void Box::initiate()
{
	this->load();
	this->buildVertexVec();
	this->indexThis();
	this->createBuffers();
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

int Box::getId()
{
	return this->id;
}

void Box::setID(int in_id)
{
	this->id = in_id;
}

void Box::setWorldMatrix(XMMATRIX in_world)
{
	this->world = in_world;
}

ID3D11ShaderResourceView * Box::getSRV()
{
	return this->srv;
}

XMMATRIX Box::getWorldMatrix() const
{
	return world;
}
