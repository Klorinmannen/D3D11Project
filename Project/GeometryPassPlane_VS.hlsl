struct vs_input
{
	float3 Position : POSITION;
	float2 uvs : TEXCOORD;
};

struct vs_output
{
	float4 Position : POSITION;
	float2 uvs : TEXCOORD;
};

vs_output VS_Entry(in vs_input input)
{
	vs_output output = (vs_output)0;

	output.uvs = input.uvs;
	output.Position = float4(input.Position, 1.0f);

	//pos = mul(pos, world);
	//pos = mul(pos, view);
	//pos = mul(pos, projection);


	//output.PositionWS = float4(input.Position, 1.0f);
	return output;
}
