

struct ps_input
{
	float3 Normal_WS : NORMALWS;
	float4 Position : SV_Position;
	float4 Position_WS : POSITIONWS;
};

struct ps_output
{
	float4 Normal : SV_Target0;
	float4 Position : SV_Target1;
	//float4 DiffuseColor : SV_Target2;
};

Texture2D Diffuse_Texture;
SamplerState Sampler;

ps_output Deferred_PS_Entry(in ps_input input)
{
	ps_output output = (ps_output)0;

	float3 reNormalized_WS = normalize(input.Normal_WS);

	output.Normal = float4(reNormalized_WS, 1.0f);
	output.Position = input.Position_WS;

	return output;
}