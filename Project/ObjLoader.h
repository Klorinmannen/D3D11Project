#ifndef OBJLOADER_H
#define OBJLOADER_H

#include"WICTextureLoader.h"
#include<string>
#include<vector>
#include<DirectXMath.h>

using namespace DirectX;

class ObjLoader
{
private:

	ID3D11Device * device;
	ID3D11DeviceContext * context;

	//containers for read data
	//std::vector<XMFLOAT3> readVertices;
	std::vector<XMFLOAT3> readVerticesNormal;
	std::vector<XMFLOAT3> readUVCoord;
	std::vector<XMFLOAT3> readVertices;
	
	//index for what vertices to use in above containers
	std::vector<int> uvIndex;
	std::vector<int> normalIndex;
	std::vector<int> vertice_index;

	//final assembled with correct xyz for each vertice, normal and uv
	std::vector<XMFLOAT3> * out_vertices;
	std::vector<XMFLOAT3> * out_normal;
	std::vector<XMFLOAT3> * out_uvCoords;
	

	ID3D11ShaderResourceView* readSRV;
	ID3D11Resource* texture;

	void addToVec(int index, std::string value);
	void buildOut();
	XMFLOAT3 storeVec(std::string & input, int xyz);
	void release();
	void reset();
	bool paraseObj(std::string & in_obj_name);
	bool getSRV(std::wstring & in_texture_name);
	void buildIndex();
public:
	ObjLoader();
	~ObjLoader();
	bool loadObjFile(	std::string &in_obj_name, 
						std::wstring &in_texture_name,
						std::vector<XMFLOAT3> *& in_vertices,
						std::vector<XMFLOAT3> *& in_vertices_normal,
						std::vector<XMFLOAT3> *& in_uv_coord,
						ID3D11ShaderResourceView *& in_srv);

	void setDevice(ID3D11Device* &in_device, ID3D11DeviceContext * &in_context);

};


#endif // !OBJLOADER_H
