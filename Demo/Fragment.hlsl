
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

	float3 tex_color = txture.Sample(sampAni, input.Tex).xyz;
	float3 ambient = float3(0.1f, 0.1f, 0.1f); //ambient
	tex_color *= ambient;

	input.Normal = normalize(input.Normal);
	float4 light_dir = normalize(light_pos - input.wPos);
	
	float cosTheta = saturate(dot(light_dir, input.Normal));
	float3 diffuse = (tex_color * light_color.xyz * light_color.w * cosTheta); //final diffuse color

	float3 light_dist_sq = (light_color.w / pow(distance(light_pos, input.wPos), 2)); //fall off light
	diffuse *= light_dist_sq;

	float4 final_color = float4(diffuse + tex_color, 1);
	
	return  saturate(final_color);
};