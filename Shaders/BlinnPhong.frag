#version 330 core
out vec4 FragColor;

in vert {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 FragPosLightSpace;
} vIn;

uniform vec4 LightColor;
uniform vec3 LightDirection;

uniform vec3 ViewPos;

uniform sampler2D ShadowMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
	{
		return 0.0;
	}

    float closestDepth = texture(ShadowMap, projCoords.xy).r; 

    float currentDepth = projCoords.z;

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
	// PCF
	vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

    return shadow;
}

void main()
{
	vec3 color = vec3(0.5f);
	vec3 ambient = vec3(0.1f) * LightColor.rgb * color;
	vec3 diffuse = max(dot(vIn.Normal, LightDirection), 0.0f) * LightColor.rgb * LightColor.a * color;

	vec3 viewDir = normalize(ViewPos - vIn.FragPos);
	vec3 H = normalize(LightDirection + viewDir);
	vec3 specular = vec3(0.5f) * pow(max(dot(vIn.Normal, H), 0.0f), 32.0f) * LightColor.rgb;

	float shadow = 1.0 - ShadowCalculation(vIn.FragPosLightSpace, LightDirection, vIn.Normal);
	//shadow = 0.0;
	FragColor = vec4(ambient + (diffuse + specular) * vec3(shadow), 1.0);

}