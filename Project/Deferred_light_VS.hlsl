
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
};

struct vs_output
{
	float4 Position : SV_Position;
};

vs_output Deferred_VS_Entry(in vs_input input)
{
	vs_output output = (vs_output)0;
	output.Position = float4(input.Position, 1.0f);
	output.Position = mul(output.Position, wvp);
}