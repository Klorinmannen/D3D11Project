#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Drawable.h"

/*
En klass som beskriver geometrin som ska ritas ut
*/

class Geometry : public Drawable
{
private:
	//D3D11 data
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;
	ID3D11Device * device;
	int vertex_number;

	int t_topology;
	int t_layout;

	//Vertex data
	int* indexArray;
	Structs::PC_Vertex* vertices; //dynamic allocated array of verticies

public:

	void setVertexArray(Structs::PC_Vertex* in_array, int nr_vertex);
	void setIndexArray(int* in_array);
	void setTopology(int in_topology);
	void setLayout(int in_layout);

public:
	Geometry(ID3D11Device * device); //Render engine is passed on to parent
	~Geometry();

	void Draw(); //implementation av renderEngines ->Draw();
	bool createBuffers();


	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	float getSizeOfVertex();
	int getTopology();
	int getLayout();
	int getNrOfVertices();

};

#endif // !GEOMETRY_H