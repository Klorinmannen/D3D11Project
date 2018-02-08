cbuffer matrixes
{
	row_major float4x4 world;
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 vp;
	row_major float4x4 wvp;
};


struct gs_in
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

struct gs_out
{
	float4 PositionCS : SV_Position;
	float3 NormalWS : NORMALWS;
	float4 PositionWS : POSITIONWS;
};


[maxvertexcount(3)]
void GS_Entry(in triangle gs_in inp[3], inout TriangleStream<gs_out> Triangles)
{

	gs_out outp = (gs_out)0;

	float4 v1;

	for (int i = 0; i < 3; i++)
	{
		v1 = mul(inp[i].Position, wvp);

		outp.PositionCS = v1;
		outp.NormalWS = inp[i].Normal;
		outp.PositionWS = inp[i].Position;

		Triangles.Append(outp);
	}

	Triangles.RestartStrip();
}