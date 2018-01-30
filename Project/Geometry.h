#pragma once
#include "Drawable.h"
/*

En klass som beskriver geometrin som ska ritas ut
*/

class Geometry : public Drawable
{
public:
	//Variables
	XMMATRIX xm_world;
	XMFLOAT3X3 xmf_world;

private:
	//D3D11 data
	ID3D11Buffer * vBuffer;
	ID3D11Buffer * iBuffer;

private:
	//functions


private:
	//Vertex data
	struct Vertex
	{
		XMFLOAT3 v_pos; //xyz
		XMFLOAT2 v_tex; //xy 
	};

	Vertex* verticies; //dynamic allocated array of verticies

public:
	Geometry(RenderEngine* engine); //Render engine is passed on to parent
	~Geometry();

	void Draw(); //implementation av renderEngines ->Draw();

	void loadVertexData(Vertex * newVerticies); // set new verticies by array
	void loadVertexData(); //overload for different uses

	void loadWorldMatrix(XMFLOAT3X3 new_matrix); //set new XMFLOAT3X3 


};