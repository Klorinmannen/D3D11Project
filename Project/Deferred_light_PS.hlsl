
cbuffer lights 
{
	float4 wPosition;
	float4 color; //w for intensity
};

//ACQUIRE TEXTURES
Texture2D NormalTexture : register(t0);
Texture2D PositionTexture : register(t1);


float4 Deferred_PS_Entry(in float4 Position_CS : SV_Position)
{
	//yadayada light ambient specular etc etc
	 
	return 
}