
Texture2D txture : register(t0);
SamplerState sampAni;

cbuffer cLight : register(b0)
{
	float4 light_pos;
	float4 light_color;
};

cbuffer VS_CONSTANT_BUFFER : register(b1)
{
	row_major matrix objectToWorldM;
	row_major matrix projectionMatrix;
	row_major matrix worldToViewM;
	row_major matrix scale;
	row_major matrix wvp; 
	row_major matrix wv;
};

struct GS_OUTPUT
{
	float4 wvpPos : SV_POSITION;
	float4 wPos : POSITION;
	float2 Tex : TEXCOORD;
	float4 Normal : NORMAL;
};

float4 PS_main(GS_OUTPUT input) : SV_Target
{

	//sample the 2D texture 
	float3 tex_color = txture.Sample(sampAni, input.Tex).xyz;

	//apply ambient to the texture color sampled
	float3 ambient = float3(0.1f, 0.1f, 0.1f); //ambient
	tex_color *= ambient;

	//Renormalize after rasterize
	input.Normal = normalize(input.Normal);
	
	//calculate the light direction
	float4 light_dir = normalize(light_pos - input.wPos);
	
	//Diffuse factor scalar
	float cosTheta = saturate(dot(light_dir, input.Normal));

	//apply the color of the light and instensity together with the diffuse part
	float3 diffuse = (tex_color * light_color.xyz * light_color.w * cosTheta); 

	//calculate the falloff light, the further away the less light
	float3 light_dist_sq = (light_color.w / pow(distance(light_pos, input.wPos), 2)); //fall off light
	diffuse *= light_dist_sq;

	//add the texture color sampled and the diffuse part together for the final color
	float4 final_color = float4(diffuse + tex_color, 1);
	
	//saturate between 0 and 1
	return  saturate(final_color);
};