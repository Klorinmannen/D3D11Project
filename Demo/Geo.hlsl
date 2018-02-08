struct GS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

struct GS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 wPos : POSITION;
	float2 Tex : TEXCOORD;
	float4 Normal : NORMAL;
};
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
		
	row_major matrix world;
	row_major matrix projection;
	row_major matrix view;
	row_major matrix scale;
	row_major matrix wvp;
	row_major matrix wv;
};


[maxvertexcount(6)]
void GS_main(in triangle GS_INPUT inP[3], inout TriangleStream<GS_OUTPUT> Triangles)
{
	GS_OUTPUT outP = (GS_OUTPUT)0;

	//calculate the face normal
		float4 v1 = inP[1].Pos - inP[0].Pos;
		float4 v2 = inP[2].Pos - inP[0].Pos;
		float3 nfn = normalize(cross(v2, v1));
		outP.Normal = mul(float4(nfn, 0.0f), world);

		//produce first triangle
			for(int i = 0; i < 3; i++)
			{
				outP.Tex = inP[i].Tex;
				outP.wPos = mul(inP[i].Pos, world);
				outP.Pos =  mul(inP[i].Pos, world);
				outP.Pos = mul(outP.Pos, view);
				outP.Pos = mul(outP.Pos, projection);
				Triangles.Append(outP);
			}

			//produce second triangle and using face normal to calculate the distance
				Triangles.RestartStrip();
				
					for(int i = 0; i < 3; i++)
					{
						outP.Tex = inP[i].Tex;
						outP.Pos = float4(inP[i].Pos.xyz + nfn.xyz * 0.5f, 1);
						outP.wPos = mul(outP.wPos, world);
						outP.Pos = mul(outP.Pos, world);
						outP.Pos = mul(outP.Pos, view);
						outP.Pos = mul(outP.Pos, projection);
						Triangles.Append(outP);
					}
 }
