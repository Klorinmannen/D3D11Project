#include"Terrain.h"

Terrain::Terrain(ID3D11Device * device) : Drawable()
{
	this->device = device;
	this->nrOfFaces = 0;
	this->nrOfVertices = 0;

	this->hM.height = 0;
	this->hM.width = 0;
	this->hM.vertexData = nullptr;

	this->vBuffer = nullptr;
	this->iBuffer = nullptr;

	this->t_layout = layout::PN; //ändra när det är dags
	this->t_topology = topology::TriangleList;
	this->loadHeightMap();
}

Terrain::~Terrain() {

	if (this->hM.vertexData != nullptr) {

		delete[] this->hM.vertexData;

	}
	
	this->vBuffer->Release();
	this->iBuffer->Release();

}

bool Terrain::loadHeightMap() {

	bool result = true;

	FILE* file = nullptr;		//Holds current position in file
	BITMAPFILEHEADER bmHeader;	//Stores information about file
	BITMAPINFOHEADER bmInfo;	//Stores information about image
	int imageSize = 0, index = 0;
	unsigned char heightValue = ' ';

	//Open file
	fopen_s(&file, "BitMaps//BitMap16x16.bmp", "rb");	//Filepath + mode to open in (rb = Read + binary data)

	if (file == nullptr) {

		exit(-1); //haha :D den lurar vem som helst

	}
	else {

		//Read bitmap header/info
		fread(&bmHeader, sizeof(BITMAPFILEHEADER), 1, file);
		fread(&bmInfo, sizeof(BITMAPINFOHEADER), 1, file);

		//get width/height
		this->hM.width = bmInfo.biWidth;
		this->hM.height = bmInfo.biHeight;

		//Get size of image in bytes, stores RGB in 3 bytes
		imageSize = (this->hM.width * this->hM.height * 3);

		//init array that stores image data
		unsigned char* bmImage = new unsigned char[imageSize];

		//Set file pointer to start
		fseek(file, bmHeader.bfOffBits, SEEK_SET);

		//Store image data
		fread(bmImage, 1, imageSize, file);

		//Close file
		fclose(file);

		//Init heightMap
		this->hM.vertexData = new DirectX::XMFLOAT3[this->hM.width * this->hM.height];

		//Read data from greyscale image, only need value from one color channel
		int k = 0;
		float heightFactor = 10.0f;

		//Read imageData into heigthMap array
		for (int j = 0; j < hM.height; j++) {

			for (int i = 0; i < hM.width; i++) {

				heightValue = bmImage[k];		//Get heightValue(R)
				index = (hM.height * j) + i;	//Find correct index

				//Store coords
				this->hM.vertexData[index].x = (float)j;
				this->hM.vertexData[index].y = (float)heightValue / heightFactor;
				this->hM.vertexData[index].z = (float)i;

				//Skip ahead of the 2 other color channels(GB)
				k += 3;

			}

		}

		delete[] bmImage;
		bmImage = 0;

	}

	return result;


}

void Terrain::createBuffers() {



	//Set number of vertices and faces
	this->nrOfVertices = this->hM.height * this->hM.width;
	this->nrOfFaces = (this->hM.height - 1) * (this->hM.width - 1) * 2;

	//Create vector and store all vertex positions and a default normal
	std::vector<Vertex> vec(this->nrOfVertices);

	for (DWORD i = 0; i < this->hM.height; ++i) {

		for (DWORD j = 0; j < this->hM.width; ++j) {

			vec[(i * this->hM.height) + j].pos = this->hM.vertexData[(i* this->hM.height) + j];
			vec[(i * this->hM.height) + j].normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

		}

	}

	//Create vector and store all index positions and texture coords
	std::vector<DWORD> indices(this->nrOfFaces * 3);
	int k = 0;
	int texUIndex = 0;
	int texVIndex = 0;

	for (DWORD i = 0; i < (this->hM.height - 1); i++) {

		for (DWORD j = 0; j < (this->hM.width - 1); j++) {
			//Triangle 1
			//Bottom left of quad
			indices[k] = (i * this->hM.width) + j;
			//vec[(i * this->hM.width) + j].tC = DirectX::XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

			//Bottom Right of quad
			indices[k + 1] = (i * this->hM.width) + j + 1;
			//vec[(i * this->hM.width) + j + 1].tC = DirectX::XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			//Top left of quad
			indices[k + 2] = ((i + 1) * this->hM.width) + j;
			//vec[((i + 1) * this->hM.width) + j].tC = DirectX::XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			//Triangle 2
			//Top left of quad
			indices[k + 3] = ((i + 1) * this->hM.width) + j;
			//vec[((i + 1) * this->hM.width) + j].tC = DirectX::XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			//Bottom right  of quad
			indices[k + 4] = (i * this->hM.width) + j + 1;
			//vec[(i * this->hM.width) + j + 1].tC = DirectX::XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			//Top right of quad
			indices[k + 5] = ((i + 1) * this->hM.width) + j + 1;
			//vec[((i + 1) * this->hM.width) + j + 1].tC = DirectX::XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

			//Skip to next quad
			k += 6;
			texUIndex++;

		}

		texUIndex = 0;
		texVIndex++;

	}

	//Copmute normals using normal averaging
	//Go through each vertex
	std::vector<DirectX::XMFLOAT3> tNormal;
	DirectX::XMFLOAT3 unNormalized = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float vecX = 0.0f, vecY = 0.0f, vecZ = 0.0f;

	DirectX::XMVECTOR e1 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR e2 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//Compute faceNormals
	for (int i = 0; i < this->nrOfFaces; ++i) {

		//Get vector of one edge of triangle(0,2)
		vecX = vec[indices[i * 3]].pos.x - vec[indices[(i * 3) + 2]].pos.x;
		vecY = vec[indices[i * 3]].pos.y - vec[indices[(i * 3) + 2]].pos.y;
		vecZ = vec[indices[i * 3]].pos.z - vec[indices[(i * 3) + 2]].pos.z;
		e1 = DirectX::XMVectorSet(vecX, vecY, vecZ, 0.0f);

		//Second edge(2, 1)
		vecX = vec[indices[(i * 3) + 2]].pos.x - vec[indices[(i * 3) + 1]].pos.x;
		vecY = vec[indices[(i * 3) + 2]].pos.y - vec[indices[(i * 3) + 1]].pos.y;
		vecZ = vec[indices[(i * 3) + 2]].pos.z - vec[indices[(i * 3) + 1]].pos.z;
		e2 = DirectX::XMVectorSet(vecX, vecY, vecZ, 0.0f);

		//Cross product of the two edges gives the un normalized face normal
		DirectX::XMStoreFloat3(&unNormalized, DirectX::XMVector3Cross(e1, e2));
		tNormal.push_back(unNormalized);


	}

	//Compute vertex normals
	DirectX::XMVECTOR normalSum = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int faceUse = 0;
	float tX = 0.0f, tY = 0.0f, tZ = 0.0f;

	for (int i = 0; i < this->nrOfVertices; ++i) {

		//Check which triangle use the vertex
		for (int j = 0; j < this->nrOfFaces; ++j) {

			if (indices[j * 3] == i ||
				indices[(j * 3) + 1] == i ||
				indices[(j * 3) + 2] == i) {

				tX = DirectX::XMVectorGetX(normalSum) + tNormal[j].x;
				tY = DirectX::XMVectorGetY(normalSum) + tNormal[j].y;
				tZ = DirectX::XMVectorGetZ(normalSum) + tNormal[j].z;

				//If a face is using the vertex, the unnormalized face normal is added
				normalSum = DirectX::XMVectorSet(tX, tY, tZ, 0.0f);
				faceUse++;

			}

		}

		//Get normal by dividing normalSum with the number of faces sharing the vertex
		normalSum = DirectX::XMVectorDivide(normalSum, DirectX::XMVectorSet(faceUse, faceUse, faceUse, faceUse));

		//Normalize sum
		normalSum = DirectX::XMVector3Normalize(normalSum);

		//Store normal in the current vertex
		vec[i].normal.x = DirectX::XMVectorGetX(normalSum);
		vec[i].normal.y = DirectX::XMVectorGetY(normalSum);
		vec[i].normal.z = DirectX::XMVectorGetZ(normalSum);

		//Clear values
		normalSum = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		faceUse = 0;

	}

	//Create Vertex Buffer
	D3D11_BUFFER_DESC vBufferDesc;
	ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));

	//Flags
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(Vertex) * this->nrOfVertices;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;

	//Data
	D3D11_SUBRESOURCE_DATA vBufferData;
	ZeroMemory(&vBufferData, sizeof(vBufferData));

	vBufferData.pSysMem = &vec[0];
	this->device->CreateBuffer(&vBufferDesc, &vBufferData, &this->vBuffer);

	//Create index buffer
	D3D11_BUFFER_DESC iBufferDesc;
	ZeroMemory(&iBufferDesc, sizeof(iBufferDesc));

	//Flags
	iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	iBufferDesc.ByteWidth = sizeof(DWORD) * (this->nrOfFaces * 3);
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;

	//Data
	D3D11_SUBRESOURCE_DATA iBufferData;
	ZeroMemory(&iBufferData, sizeof(iBufferData));

	iBufferData.pSysMem = &indices[0];
	this->device->CreateBuffer(&iBufferDesc, &iBufferData, &this->iBuffer);

}

int Terrain::getNrOfFaces(void) const {

	return this->nrOfFaces;

}

int Terrain::getNrOfVertices(void) {

	return this->nrOfVertices;

}

ID3D11Buffer* Terrain::getVertexBuffer() {

	return this->vBuffer;

}

ID3D11Buffer* Terrain::getIndexBuffer() {

	return this->iBuffer;

}

float Terrain::getSizeOfVertex()
{
	return sizeof(Vertex);
}

int Terrain::getTopology()
{
	return this->t_topology;
}

int Terrain::getLayout()
{
	return this->t_layout;
}

XMMATRIX Terrain::getWorldMatrix() const
{
	return (DirectX::XMMatrixScaling(10.0f, 10.0f, 10.0f) * DirectX::XMMatrixTranslation(-100.0f, -100.0f, -100.0f));
}
