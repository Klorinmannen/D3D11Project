
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
	float4 Position_CS : SV_Position;
	float3 Normal_WS : NORMAL_WS;
	float4 Position_WS : POSITION_WS;
};

vs_output Deferred_VS_Entry(in vs_input input)
{
	vs_output output = (vs_output)0; //init

	output.Normal_WS = mul(input.Normal, world);
	float4 pos = float4(input.Position, 1.0f);
	output.Position_CS = mul(pos, wvp);
	output.Position_WS = mul(pos, world);
	return output;
}
