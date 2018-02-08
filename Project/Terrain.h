#ifndef TERRAIN_H
#define TERRAIN_H

#include <DirectXMath.h>
#include <d3d11.h>
#include <stdio.h>
#include <vector>
#include "Drawable.h"

class Terrain : public Drawable {

private:

	struct HeightMap {

		int width;
		int height;
		DirectX::XMFLOAT3* vertexData;

	};
	//Vertex structure
	struct Vertex {

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;

	};

	HeightMap hM;
	int nrOfFaces;
	int nrOfVertices;

	int t_topology;
	int t_layout;

	//Buffers
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;

	ID3D11Device * device;

public:

	Terrain(ID3D11Device * device);
	~Terrain();

	bool loadHeightMap();
	void createBuffers();

	//Get
	int getNrOfFaces(void) const;
	//this->nrOfFaces * 3
	int getNrOfVertices(void);
	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	float getSizeOfVertex();
	int getTopology();
	int getLayout();
	XMMATRIX getWorldMatrix() const;
};

#endif // !TERRAIN_H
