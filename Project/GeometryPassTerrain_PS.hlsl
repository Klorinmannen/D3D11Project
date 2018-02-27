
Texture2D grass : register(t0);
Texture2D stone : register(t1);
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

ps_output PS_Entry(in ps_input input)
{
	ps_output output = (ps_output)0;

	input.NormalWS = normalize(input.NormalWS);

	output.Normal = input.NormalWS;
	output.Position = input.PositionWS;
	output.Texture = grass.Sample(samp, input.uvs);
	output.Texture += stone.Sample(samp, input.uvs).xyz;
	////bla bla homo homo något
	//input.lpos.xyz /= input.lpos.w;

	////transform from cs to ts
	//input.lpos.x = input.lpos.x / 2 + 0.5f;
	//input.lpos.y = input.lpos.y / 2 + 0.5f;
	output.lpos = input.lpos;
	return output;
}