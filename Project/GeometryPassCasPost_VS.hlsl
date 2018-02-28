cbuffer matrixes
{
	row_major float4x4 world;
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 inverseWorld;
	row_major float4x4 wvp;
	row_major float4x4 vpLight;
};

struct vs_input
{
	float3 Position : POSITION;
	float2 uvs : TEXCOORD;
};

struct vs_output
{
	float4 Position : SV_Position;
	float2 uvs : TEXCOORD;
};

vs_output VS_Entry(in vs_input input)
{
	vs_output output = (vs_output)0;

	output.uvs = input.uvs;
	float4 pos = float4(input.Position, 1.0f);
	pos = mul(pos, wvp);
	output.Position = pos;

	return output;
}
