
Texture2D cat: register(t0);
SamplerState samp : register(s0);

struct ps_input
{
	float4 PositionSS : SV_Position;
	float2 uvs : TEXCOORD;

};

struct ps_output
{
	float3 Texture : SV_Target;
};

//Gaussian Blurr
static float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };


ps_output PS_Entry(in ps_input input)
{
	ps_output output = (ps_output)0;

	//Gaussian Blurr
	float w = 1920;
	float h = 1200;

	float offset_weight = 1.0f / w * h;
	float3 color = cat.Sample(samp, input.uvs) * weights[0];
	float3 color1 = cat.Sample(samp, input.uvs);
	for (int i = 1; i < 5; i++)
	{
		color += cat.Sample(samp, input.uvs + float2(0.0f, offset_weight * i)) * weights[i];
		color += cat.Sample(samp, input.uvs - float2(0.0f, offset_weight * i)) * weights[i];
	}

	output.Texture = color;
	return output;
}