cbuffer matrixes
{
	row_major float4x4 world;
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 inverseWorld;
	row_major float4x4 wvp;
	row_major float4x4 vpLight;
};

struct gs_in
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 uvs : TEXCOORD;
};

struct gs_out
{
	float4 PositionCS : SV_Position;
	float3 NormalWS : NORMALWS;
	float4 PositionWS : POSITIONWS;
	float2 uvs : TEXCOORD;
	float4 lpos : TEXCOORD1;
};


[maxvertexcount(3)]
void GS_Entry(in triangle gs_in inp[3], inout TriangleStream<gs_out> Triangles)
{

	gs_out outp = (gs_out)0;

	//Backface culling
	float4 e1 = (inp[1].Position - inp[0].Position);
	float4 e2 = (inp[2].Position - inp[0].Position);
	float3 fNormal = normalize(cross(e1, e2).xyz);

	for (int i = 0; i < 3; i++)
	{
		outp.PositionCS = mul(inp[i].Position, wvp);
		outp.NormalWS = normalize(mul(inp[i].Normal, world));
		outp.PositionWS = mul(inp[i].Position, world);
		outp.uvs = inp[i].uvs;
		outp.lpos = mul(inp[i].Position, mul(world, vpLight));
		Triangles.Append(outp);
	}

	Triangles.RestartStrip();
}