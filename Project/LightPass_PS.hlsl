
SamplerState pSamp;

cbuffer lights
{
	float4 direction;
	float4 lColor; //w for intensity
	float4 camPos;
	float4 lightPos;
};

//ACQUIRE TEXTURES
Texture2D NormalTexture : register(t0);
Texture2D PositionTexture : register(t1);
Texture2D TexToSample : register(t2);
Texture2D lposSample : register(t3);
Texture2D shadowMap : register(t4);
Texture2D depthMap : register(t5);


float4 PS_Entry(in float4 PositionCS : SV_Position) : SV_Target0
{
	float shineFactor = 1000.0f;
	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	//sample srvs
	int3 indices = int3(PositionCS.xy, 0);
	float3 wNormal = normalize(NormalTexture.Load(indices).xyz);
	float4 wPos = PositionTexture.Load(indices).xyzw;
	float3 tColor = TexToSample.Load(indices).xyz;
	float4 lpos = lposSample.Load(indices).xyzw;

	//calculate materialAmbient
	float3 materialAmbient = ambient * tColor;


	//****shadows****
	//homogena
	lpos.xyz /= lpos.w;

	//from -1 to 1 -> 0 to 1 clipspace to uvs
	lpos.x = lpos.x / 2 + 0.5f;
	lpos.y = lpos.y / -2 + 0.5f;

	//sample shadowMap with lpos.xy
	float shadowR = shadowMap.Sample(pSamp, lpos.xy).r;

	//compare shadowmap depth to lightPos depth seen from camera view
	if (shadowR < lpos.z)
	{
		return float4(materialAmbient, 1.0f);
	}

	//****diffuse shading****
	//from lights direction
	float3 lightDir = direction.xyz;
	lightDir = normalize(lightDir);
	//diffuse factor scalar cosTheta
	float cosTheta = saturate(dot(lightDir, wNormal));
	//apply texture color * light color * diffuse factor * intensity     //calculate the falloff light
	float3 diffuseTotal;
	diffuseTotal = tColor * lColor.xyz * lColor.w * cosTheta * (lColor.w / (pow(distance(wPos.xyz, lightPos.xyz), 1))); 



	//****specular shading****
	//vector from texturepoint to camPosition
	float3 toLight = lightPos.xyz - wPos.xyz;
	toLight = normalize(toLight);
	toLight = -toLight;

	//reflect function yields reflect vector v = i - 2 * n * dot(i.n) 
	float3 reflection = reflect(toLight, wNormal);
	//float3 reflection = 2.0f * dot(wNormal, toLight) * wNormal - toLight;
	reflection = normalize(reflection);

	//vector from texel to camera
	float3 toCam = camPos.xyz - wPos.xyz;
	toCam = normalize(toCam);

	//cosine between toCam and reflection
	float cos = max(0.0, dot(reflection, toCam));

	//apply the "shine" factor
	cos = pow(cos, shineFactor);

	//final spec color
	float3 specularColor; // = materialAmbient;
	specularColor = lColor.xyz * cos; // * tColor;

	 
	//final out color
	float3 finalOut = materialAmbient + diffuseTotal + specularColor;
	
	//float3 finalOut = specularColor;
	float4 test1 = float4(lColor.x, 0.0f, 0.0f, 1.0f);
	float4 test2 = float4(0.0f, lColor.y, 0.0f, 1.0f);
	float4 test3 = float4(0.0f, 0.0f, lColor.z, 1.0f);


	return  float4(finalOut, 1.0f); //saturate(float4(finalOut, 1.0f));
	//return shadowR;
	//return lpos;;
	//return float4(tNormal, 1.0f);
}	