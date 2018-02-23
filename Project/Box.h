#ifndef BOX_H
#define BOX_H

#include "ObjLoader.h"
#include "Drawable.h"
#include "Settings.h"

class Box : public Drawable
{
private:

	int topology;
	int layout;
	int id;

	struct PTN_Vertex
	{
		XMFLOAT3 v_pos;
		XMFLOAT3 v_normal;
		XMFLOAT3 uvs;
	};

	XMMATRIX world = XMMatrixScaling(20, 20, 20) * XMMatrixTranslation(6400.0f, 3400.0f, 6400.0f);

	std::vector<PTN_Vertex> vertexVec;
	PTN_Vertex vertex;


	ObjLoader * loader;

	std::vector<XMFLOAT3> * vertices;
	std::vector<XMFLOAT3> * normals;
	std::vector<XMFLOAT3> * uvs;
	std::vector<int> * index;

	ID3D11ShaderResourceView * srv;
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;
	ID3D11Device *  device;

	std::string objName;
	std::wstring picName;


	void buildVertexVec();
	void load();
	void createBuffers();
	void indexThis();
public:
	Box();
	~Box();
	void initiate();
	void setLoader(ObjLoader * in_loader);
	void setDevice(ID3D11Device * in_device);
	ID3D11Buffer * getVertexBuffer();
	ID3D11Buffer * getIndexBuffer();
	XMMATRIX getWorldMatrix() const;
	float getSizeOfVertex();
	int getTopology();
	int getLayout();
	int getNrOfVertices();
	int getId();
	void setID(int in_id);
	void setWorldMatrix(XMMATRIX in_world);
	ID3D11ShaderResourceView * getSRV();

};



#endif // !BOX_H
