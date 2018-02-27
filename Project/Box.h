#ifndef BOX_H
#define BOX_H

#include "ObjLoader.h"
#include "Drawable.h"

class Box : public Drawable
{
private:

	int topology;
	int layout;

	struct PTN_Vertex
	{
		XMFLOAT3 v_pos;
		XMFLOAT3 v_normal;
		XMFLOAT3 uvs;
	};

	XMMATRIX m_world;
	XMMATRIX m_translation;
	XMMATRIX m_scale;

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
	void findMidPoint();
	void transformMidPoint();
	void update();
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
	void setTranslationMatix(XMMATRIX in_trans);
	void setScaleMatrix(XMMATRIX in_scale);
	ID3D11ShaderResourceView * getSRV();

	bool operator < (const Box &other) const;
};



#endif // !BOX_H
