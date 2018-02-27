#ifndef TERRAIN_H
#define TERRAIN_H
#include"WICTextureLoader.h"
#include <stdio.h>
#include <vector>
#include "Drawable.h"

class Terrain
{
private:

	struct HeightMap {

		int width;
		int height;
		DirectX::XMFLOAT3** vertexData;

	};
	//Vertex structure
	struct Vertex {

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uvs;

	};

	XMMATRIX world = XMMatrixTranslation(1.0f, 1.0f, 1.0f) * XMMatrixScaling(1000.0f, 100.0f, 1000.0f);

	float scalingValue;
	float offsetValue;

	HeightMap hM;
	int nrOfFaces;
	int nrOfVertices;
	int t_topology;
	int t_layout;
	int valuesPerVertex;
	int nrOfSRVS;

	float scale = 1.0f;

	//Buffers
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;
	ID3D11ShaderResourceView * srv[2];
	ID3D11Device * device;

	//Texture
	ID3D11SamplerState* sampState;
	ID3D11ShaderResourceView* grassView;
	ID3D11ShaderResourceView* stoneView;



public:

	Terrain(ID3D11Device * device);
	~Terrain();

	bool loadHeightMap();
	void createBuffers();
	void createTexture();
	void createSamplerState();

	//Get
	int getNrOfFaces(void) const;
	//this->nrOfFaces * 3
	int getNrOfVertices(void);
	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	float getSizeOfVertex();
	int getTopology();
	int getLayout();
	int getNrOfSRVS();
	XMMATRIX getWorldMatrix() const;
	ID3D11ShaderResourceView ** getSRV();
	ID3D11SamplerState* getSamplerState();
};

#endif // !TERRAIN_H
