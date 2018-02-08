
cbuffer lights
{
	float4 wPosition;
	float4 Color; //w for intensity
};

struct ps_input
{
	float4 PositionSS : SV_Position;
	float3 NormalWS : NORMALWS;
	float4 PositionWS : POSITIONWS;
};

struct ps_output
{
	float3 Normal : SV_Target0;
	float4 Position : SV_Target1;
	//float4 DiffuseColor : SV_Target2;
};

ps_output PS_Entry(in ps_input input)
{
	ps_output output = (ps_output)0;

	//float3 reNormalizedWS = saturate(normalize(input.NormalWS));

	output.Normal.xz = input.NormalWS.xz;
	output.Normal.y = -input.NormalWS.y;
	output.Position = input.PositionWS;

	return output;
}