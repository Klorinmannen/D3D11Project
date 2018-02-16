#ifndef Settings_H
#define Settings_H

#include <DirectXMath.h>

//add more if needed depending on objects etc etc.
enum topology{TriangleList, TriangleStrip, TriangleStripAdj, LineStrip, LineList};
enum layout{PTN = 0, PC = 1, PN = 2, Pos = 3};

namespace Structs
{
	struct PC_Vertex
	{
		DirectX::XMVECTOR v_Pos;
		DirectX::XMVECTOR v_Color;
	};



}
#endif
