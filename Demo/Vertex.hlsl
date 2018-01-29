

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	row_major matrix objectToWorldM;
	row_major matrix projectionMatrix;
	row_major matrix worldToViewM;
	row_major matrix scale;
	row_major matrix wvp;
	row_major matrix wv;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Tex = input.Tex;
	output.Pos = float4(input.Pos, 1);
	// output.Color = input.Color;
	//output.Pos = mul(output.Pos, scale);
	// output.Pos = mul(output.Pos, objectToWorldM);	
	// output.Pos = mul(output.Pos, worldToViewM);
	// output.Pos = mul(output.Pos, projectionMatrix);

	//output.Pos = mul(output.Pos, wvp);

	return output;
}