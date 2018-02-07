
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

struct vs_output
{
	float4 PositionCS : SV_Position;
	float3 NormalWS : NORMALWS;
	float4 PositionWS : POSITIONWS;
};

vs_output VS_Entry(in vs_input input)
{
	vs_output output = (vs_output)0;

	output.NormalWS = input.Normal;
	float4 pos = float4(input.Position, 1.0f);
	
	pos = mul(pos, world);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	output.PositionCS = pos;
	output.PositionWS = pos;
	return output;
}
