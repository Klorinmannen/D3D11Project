#ifndef TERRAIN_H
#define TERRAIN_H

#include <DirectXMath.h>
#include <d3d11.h>
#include<stdio.h>
#include<vector>

class Terrain {

private:

	struct HeightMap {

		int width;
		int height;
		DirectX::XMFLOAT3* vertexData;

	};

	HeightMap hM;
	int nrOfFaces;
	int nrOfVertices;

	//Buffers
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;

public:

	Terrain();
	~Terrain();

	bool loadHeightMap(char* url);
	void createBuffers();

	//Get
	DirectX::XMFLOAT3 getVertexPos(int index) const;
	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();

};

#endif // !TERRAIN_H
