#ifndef TERRAIN_H
#define TERRAIN_H

#include <DirectXMath.h>
#include <d3d11.h>
#include<stdio.h>
#include<vector>
#include"Drawable.h"

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
	Vertex vert_struct;

	HeightMap hM;
	int nrOfFaces;
	int nrOfVertices;

	topology t_topology;
	layout t_layout;

	//Buffers
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;

public:

	Terrain();
	~Terrain();

	bool loadHeightMap();
	void createBuffers(ID3D11Device* device);

	//Get
	int getNrOfFaces(void) const;
	int getNrOfVertices(void) const;
	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	float getSizeOfVertex();
	int getTopology();
	int getLayout();

	void Draw();
};

#endif // !TERRAIN_H
