#include "ObjLoader.h"
#include<fstream>
#include<algorithm>

ObjLoader::ObjLoader()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	this->device = nullptr;
	this->reset();
}

ObjLoader::~ObjLoader()
{
	this->device = nullptr;
	CoUninitialize();
	this->release();
	this->reset();
}

bool ObjLoader::paraseObj(std::string & in_obj_name)
{
	//open .obj manage as a textfile, read row by row til eof and parse the read content into vectors;

	std::ifstream file;
	std::string input = "";
	file.open(in_obj_name);

	int debugCounter = -1;

	if (file.bad())
	{
		return false;
	}
	
	while (1)
	{

		if (file.eof())
		{
			//break if we reach eof
			break;
		}

		//reads line by line into input object
		std::getline(file, input);

		//check comment
		if (input[0] != '#' && input[0] != '\0')
		{
			if (input.find("vn", 0, 2) < 2)
			{
				//vertex Normal

				this->readVerticesNormal.push_back(this->storeVec(input, 3));

			}
			else if (input.find("vt", 0, 2) < 2)
			{
				//texture coord

				this->readUVCoord.push_back(this->storeVec(input, 2));
			}
			else if (input.find("v", 0, 1) < 1)
			{
				//texture coordinate

				this->readVertices.push_back(this->storeVec(input, 3));
			}
			else if (input.find("f", 0, 1) < 1)
			{
				//removes / ws and f from the current string
				//input.erase(std::remove(input.begin(), input.end(), '/'), input.end()); 
				//input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
				input.erase(0, 2);

				
				if (input.back() == ' ')
				{
					input.back() = '\0';
				}

				std::string value = "";
				int index = 0;

				
				for (int i = 0; i < input.size(); i++)
				{
					switch (input[i])
					{
					case '/':
						//new index
						this->addToVec(index, value);
						index++;
						value.clear();
						break;
					case ' ':
						//new face
						this->addToVec(index, value);
						index = 0;
						value.clear();
						break;
					default:
						//numbers
						value += input[i];
						break;
					}
				}
				this->addToVec(index, value);
				this->face += 4;
				this->triangulate();


			}
		}

	}

	//build out vectors
	this->buildOut();
	file.close();
	return true;
}


void ObjLoader::triangulate()
{
	//use the current added vertices, replicate the two first once to create two triangles
	this->vertice_index.push_back(this->vertice_index[this->face - 4]);
	this->vertice_index.push_back(this->vertice_index[this->face - 2]);

	//reproduce normals for replicates
	this->normalIndex.push_back(this->normalIndex[this->face - 4]);
	this->normalIndex.push_back(this->normalIndex[this->face - 2]);

	//reproduce uvs
	this->uvIndex.push_back(this->uvIndex[this->face - 4]);
	this->uvIndex.push_back(this->uvIndex[this->face - 2]);

	//increase nr faces added by two
	face += 2;
}

void ObjLoader::addToVec(int index, std::string value)
{
	switch (index)
	{
	case 0:
		this->vertice_index.push_back(stoi(value));
		break;
	case 1:
		this->uvIndex.push_back(stoi(value));
		break;
	case 2:
		this->normalIndex.push_back(stoi(value));
		break;
	}

}

void ObjLoader::buildOut()
{
	//building out vectors

	XMFLOAT3 vec3;
	int index = 0;


	for (size_t i = 0; i < this->vertice_index.size(); i++)
	{
		//get index. Indexing starts at 1 in .obj, -1
		index = this->vertice_index[i] - 1;

		//use index to find correct vertex
		vec3 = this->readVertices[index];

		//store vertex into return vector
		this->out_vertices->push_back(vec3);
	}

	for (size_t i = 0; i < this->uvIndex.size(); i++)
	{
		index = this->uvIndex[i];
		vec3 = this->readUVCoord[index - 1];
		this->out_uvCoords->push_back(vec3);
	}

	for (size_t i = 0; i < this->normalIndex.size(); i++)
	{
		index = this->normalIndex[i];
		vec3 = this->readVerticesNormal[index - 1];
		this->out_normal->push_back(vec3);
	}


}

XMFLOAT3 ObjLoader::storeVec(std::string & input, int xyz)
{

	XMFLOAT3 vec3;
	vec3 = XMFLOAT3(0, 0, 0);
	std::string value = "";
	int turn = 0;

	if (xyz == 3)
	{
		//iterate string, iterating backwards
		for (int i = input.size() - 1; i >= 0; i--)
		{
			//check for whitespaces
			if (input[i] != ' ')
			{
				//use insert, iterating backwards
				value.insert(0, 1, input[i]);
			}
			else
			{
				//input to vector z y x order
				switch (turn)
				{
				case 0:
					vec3.z = atof(value.c_str());
					break;
				case 1:
					vec3.y = atof(value.c_str());
					break;
				case 2:
					vec3.x = atof(value.c_str());
					i = -1; //break loop
					turn = 0;
					break;
				}
				turn++;
				value.clear();
			}
		}
	}
	else if (xyz == 2)
	{
		//iterate string, iterating backwards
		for (int i = input.size() - 1; i >= 0; i--)
		{
			//check for whitespaces
			if (input[i] != ' ')
			{
				//use insert, iterating backwards
				value.insert(0, 1, input[i]);
			}
			else
			{
				//input to vector y x order
				switch (turn)
				{
				case 0:
					vec3.y = atof(value.c_str());
					break;
				case 1:
					vec3.x = atof(value.c_str());
					i = -1; //break loop
					turn = 0;
					break;
				}
				turn++;
				value.clear();
			}
		}
	}
	//return complete vector
	return vec3;
	
}

bool ObjLoader::getSRV(std::wstring & in_texture_name)
{
	//open texture file with DirectXToolKit function

	HRESULT hr;
	hr = CreateWICTextureFromFile(this->device, in_texture_name.c_str(), nullptr, &this->readSRV);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void ObjLoader::buildIndex()
{
	readVertices.clear();
	readVerticesNormal.clear();
	readUVCoord.clear();

	readVertices.push_back(this->out_vertices->front());
	readVerticesNormal.push_back(this->out_normal->front());
	readUVCoord.push_back(this->out_uvCoords->front());

	std::swap(this->out_vertices->front(), this->out_vertices->back());
	this->out_vertices->pop_back();

	std::swap(this->out_uvCoords->front(), this->out_uvCoords->back());
	this->out_uvCoords->pop_back();

	std::swap(this->out_normal->front(), this->out_normal->back());
	this->out_normal->pop_back();


}

void ObjLoader::release()
{
	//delete allocated memory not used
	delete this->out_normal;
	delete this->out_uvCoords;
	delete this->out_vertices;
}

void ObjLoader::reset()
{
	//reset ptrs
	this->readSRV = nullptr;
	this->out_vertices = nullptr;
	this->out_uvCoords = nullptr;
	this->out_normal = nullptr;

	//clear vectors of old information
	this->readUVCoord.clear();
	this->readVerticesNormal.clear();
	this->readVertices.clear();
	this->uvIndex.clear();
	this->normalIndex.clear();
	this->vertice_index.clear();

	this->face = 0;
}

bool ObjLoader::loadObjFile(std::string & in_obj_name,
	std::wstring & in_texture_name,
							std::vector<XMFLOAT3> *& in_vertices, 
							std::vector<XMFLOAT3> *& in_vertices_normal,
							std::vector<XMFLOAT3> *& in_uv_coord,
							ID3D11ShaderResourceView *& in_srv)
{
	//allocate new vectors 
	this->out_vertices = new std::vector<XMFLOAT3>;
	this->out_uvCoords = new std::vector<XMFLOAT3>;
	this->out_normal = new std::vector<XMFLOAT3>;

	if (this->paraseObj(in_obj_name) && this->getSRV(in_texture_name))
	{
		in_srv = this->readSRV; 
		in_vertices = this->out_vertices;
		in_vertices_normal = this->out_normal;
		in_uv_coord = this->out_uvCoords;

		this->reset();
		return true;
	}
	else
	{
		//failed something, release, reset

		this->release();
		this->reset();

		return false;
	}
}

void ObjLoader::setDevice(ID3D11Device * &in_device, ID3D11DeviceContext * &in_context)
{
	this->device = in_device;
	this->context = in_context;
}
