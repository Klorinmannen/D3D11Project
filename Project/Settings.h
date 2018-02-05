#ifndef settings_H
#define settings_H


//add more if needed depending on objects etc etc.
enum topology{TriangleList = 0};
enum layout{PTN = 0, PC = 1, PN = 2};

namespace Colors
{
	float white[4] = { 1, 1, 1, 1 };
	float black[4] = { 0, 0, 0, 1 };
}

struct PC_Vertex
{
	XMVECTOR v_Pos; 
	XMVECTOR v_Color; 
};
#endif
