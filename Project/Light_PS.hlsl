
cbuffer lights 
{
	float4 wPosition;
	float4 Color; //w for intensity
};

//ACQUIRE TEXTURES
Texture2D NormalTexture : register(t0);
Texture2D PositionTexture : register(t1);

struct ps_in
{
	float4 Position : SV_Position;
	float4 Normal : NORMAL;
};



float4 PS_Entry(in float4 PositionCS : SV_Position) : SV_Target0
{

	//yadayada light ambient specular etc etc
	int3 indices = int3(PositionCS.xy, 0);
	float3 Normal = NormalTexture.Load(indices);


	return float4(Normal, 1);
}