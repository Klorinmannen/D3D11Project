
cbuffer lights 
{
	float4 wPosition;
	float4 Color; //w for intensity
};

//ACQUIRE TEXTURES
Texture2D NormalTexture : register(t0);
Texture2D PositionTexture : register(t1);
Texture2D SampledTexture : register(t2);

float4 PS_Entry(in float4 PositionCS : SV_Position) : SV_Target0
{
	float4 ambient = float4(0.2, 0.2, 0.2, 1);
	//yadayada light ambient specular etc etc
	int3 indices = int3(PositionCS.xy, 0);
	float3 Normal = NormalTexture.Load(indices).xyz;
	float3 pos = PositionTexture.Load(indices).xyz;
	float3 txtC = SampledTexture.Load(indices).xyz;

	Normal.y = Normal.y;

	float3 total = txtC;
	return float4(total, 1.0f);
}