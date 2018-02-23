
cbuffer lights
{
	float4 direction;
	float4 lColor; //w for intensity
	float4 camPos;
};

//ACQUIRE TEXTURES
Texture2D NormalTexture : register(t0);
Texture2D PositionTexture : register(t1);
Texture2D TexToSample : register(t2);
Texture2D shadowMap : register(t3);

float4 PS_Entry(in float4 PositionCS : SV_Position) : SV_Target0
{
	float shineFactor = 900.0f;
	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	//sample srvs
	int3 indices = int3(PositionCS.xy, 0);
	float3 tNormal = normalize(NormalTexture.Load(indices).xyz);
	float4 tPos = PositionTexture.Load(indices).xyzw;
	float3 tColor = TexToSample.Load(indices).xyz;
	float shadowX = shadowMap.Load(indices).x;
	//calculate materialAmbient
	float3 materialAmbient = ambient * tColor;



	//diffuse shading
	//from lights direction
	float3 lightDir = direction.xyz;
	//diffuse factor scalar cosTheta
	float cosTheta = saturate(dot(lightDir, tNormal));
	//apply texture color * light color * diffuse factor * intensity     //calculate the falloff light
	float3 diffuseTotal = materialAmbient;
	diffuseTotal += (tColor * lColor.xyz * lColor.w * cosTheta); // *(lColor.w / pow(distance(direction, tPos), 1));

	//specular
	//vector from texturepoint to camPosition
	float3 texToCam = normalize(camPos.xyz - tPos.xyz);
	//reflect function yields reflect vector v = i - 2 * n * dot(i.n) 
	float3 reflection = reflect(-lightDir, tNormal);
	//cosine between texToCam and reflection vector
	float cos = clamp(dot(texToCam, reflection), 0, 1);
	float SpecularPower = pow(cos, shineFactor);
	float3 specularTotal = tColor * lColor.xyz * lColor.w * SpecularPower *(lColor.w / pow(distance(direction, tPos), 1));
	 
	//final out color
	float3 finalOut = diffuseTotal; // + specularTotal;
	float4 test1 = float4(lColor.x, 0.0f, 0.0f, 1.0f);
	float4 test2 = float4(0.0f, lColor.y, 0.0f, 1.0f);
	float4 test3 = float4(0.0f, 0.0f, lColor.z, 1.0f);


	return  float4(finalOut, 1.0f);//saturate(float4(finalOut, 1.0f));
}