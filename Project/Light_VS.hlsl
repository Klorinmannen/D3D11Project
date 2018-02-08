
struct vs_input
{
	float3 Position : POSITION;
};

float4 VS_Entry(in vs_input input) :  SV_Position
{

	//transform vertex into clipspace
	return float4(input.Position, 1.0f);
}