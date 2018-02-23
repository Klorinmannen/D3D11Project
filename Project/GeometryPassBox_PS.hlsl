


Texture2D box : register(t0);
SamplerState samp;

struct ps_input
{
	float4 PositionSS : SV_Position;
	float3 NormalWS : NORMALWS;
	float4 PositionWS : POSITIONWS;
	float2 uvs : TEXCOORD;
};

struct ps_output
{
	float3 Normal : SV_Target0;
	float4 Position : SV_Target1;
	float3 Texture : SV_Target2;
};

ps_output PS_Entry(in ps_input input)
{
	ps_output output = (ps_output)0;

	input.NormalWS = normalize(input.NormalWS);

	output.Normal = input.NormalWS;
	output.Position = input.PositionWS;
	output.Texture = box.Sample(samp, input.uvs).xyz;
	return output;
}