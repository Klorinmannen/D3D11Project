
cbuffer matrixes
{
	row_major float4x4 world;
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 wvp;
};

struct vs_input
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

float4 VS_Entry(in vs_input input) :  SV_Position
{

	return mul(float4(input.Position, 1), wvp);
}