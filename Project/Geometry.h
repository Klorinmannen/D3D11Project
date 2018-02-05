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
	int vertex_number;

	topology t_topology;
	layout t_layout;

public:
	//Vertex data
	int* indexArray;
	PC_Vertex* vertices; //dynamic allocated array of verticies
	void setVertexArray(PC_Vertex* in_array, int nr_vertex);
	void setIndexArray(int* in_array);

public:
	Geometry(RenderEngine* engine); //Render engine is passed on to parent
	~Geometry();

	void Draw(); //implementation av renderEngines ->Draw();
	bool createBuffers();

	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	float getSizeOfVertex();
	topology getTopology();
	layout getLayout();
	int getNrOfVertices();

};

#endif // !GEOMETRY_H