#ifndef PLANE_H
#define PLANE_H
#include<vector>
#include"Drawable.h"

class Plane : public Drawable
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

	XMMATRIX m_world;
	XMMATRIX m_translation;
	XMMATRIX m_scale;
	XMMATRIX m_rotation;

	ID3D11Device * device;
	ID3D11Buffer * vBuffer;
	ID3D11Buffer * iBuffer;
	ID3D11SamplerState * sampState;
	void createVertices();
	void createBuffers();
	void createSamplerState();
	void findMidPoint();
	void transformMidPoint();
	void update();

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
	void setRotationMatrix(XMMATRIX in_rot);
	void setTranslationMatix(XMMATRIX in_trans);
	void setScaleMatrix(XMMATRIX in_scale);
	int getTopology();
	int getLayout();
	int getSizeOfVertex();
	int getNrOfVertices();
	bool operator < (const Plane &other) const;

};
#endif // !PLANE_H

