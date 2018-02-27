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
	float epsilon = 0.000001f;
	float4 posWS[3];
	float4 posCS[3];
	float4 posL[3];
	for (int i = 0; i < 3; i++)
	{
		posWS[i] = mul(inp[i].Position, world);
		posCS[i] = mul(inp[i].Position, wvp);
		posL[i] = mul(inp[i].Position, mul(world, vpLight));
	}

	float3 e1 = (posCS[1].xyz - posCS[0].xyz);
	float3 e2 = (posCS[2].xyz - posCS[0].xyz);
	float3 fNormal = normalize(cross(e1, e2).xyz);


	if (fNormal.z < epsilon)
	{
		e1 = (posWS[1].xyz - posWS[0].xyz);
		e2 = (posWS[2].xyz - posWS[0].xyz);
		fNormal = normalize(cross(e1, e2).xyz);

		for (int i = 0; i < 3; i++)
		{
			outp.PositionCS = posCS[i];
			outp.NormalWS = fNormal;
			outp.PositionWS = posWS[i];
			outp.uvs = inp[i].uvs;
			outp.lpos = posL[i];
			Triangles.Append(outp);
		}
	}
	else
	{
		Triangles.Append(outp);
	}
}