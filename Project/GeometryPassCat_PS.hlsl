


Texture2D cat: register(t0);
SamplerState samp : register(s0);

struct ps_input
{
	float4 PositionSS : SV_Position;
	float3 NormalWS : NORMALWS;
	float4 PositionWS : POSITIONWS;
	float2 uvs : TEXCOORD;
	float4 lpos : TEXCOORD1;
};

struct ps_output
{
	float3 Normal : SV_Target0;
	float4 Position : SV_Target1;
	float3 Texture : SV_Target2;
	float4 lpos : SV_Target3;
};

//Gaussian Blurr
static float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };


ps_output PS_Entry(in ps_input input)
{
	ps_output output = (ps_output)0;

	input.NormalWS = normalize(input.NormalWS);

	output.Normal = input.NormalWS;
	output.Position = input.PositionWS;
	output.lpos = input.lpos;

	//Gaussian Blurr
	float w = 1920;
	float h = 1200;

	float offset_weight = 1.0f / w * h;
	float3 color = cat.Sample(samp, input.uvs) * weights[0];
	float3 color1 = cat.Sample(samp, input.uvs);
	for (int i = 1; i < 5; i++)
	{
		color += cat.Sample(samp, input.uvs + float2(offset_weight * i, 0.0f)) * weights[i];
		color += cat.Sample(samp, input.uvs - float2(offset_weight * i, 0.0f)) * weights[i];
	}

	output.Texture = color;
	return output;
}