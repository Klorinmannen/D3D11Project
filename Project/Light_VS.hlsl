cbuffer matrixes
{
	row_major float4x4 world;
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 inverseWorld;
	row_major float4x4 wvp;
	row_major float4x4 vpLight;
};


float4 VS_Entry(in float3 Position : POSITION) : SV_Position
{

	return float4(Position, 1.0f);;
}