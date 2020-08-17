#shader vertex
#version 330 core

layout(location = 0) in int data;

out vec3 normal;
out vec3 camDir;
out vec4 fragPosSunViewSpace;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform ivec2 u_ChunkOffset;
uniform vec3 u_CamPos;
uniform mat4 u_SunViewProjectionMatrix;

const vec3 NORMALS[6] = vec3[6](
	vec3(-0.7f,0.0f,0.0f),
	vec3(0.7f,0.0f,0.0f),
	vec3(0.0f,0.0f,-0.6f),
	vec3(0.0f,0.0f, 0.6f),
	vec3(0.0f,-0.8f,0.0f),
	vec3(0.0f,0.8f,0.0f)
);

void main()
{
	int x = (data & 0x1F) + u_ChunkOffset.x;
	int z = ((data >> 5) & 0x1F) + u_ChunkOffset.y;
	int y = ((data >> 10) & 0x1FF);
	int normalIndex = ((data >> 27) & 0x7);
	normal = NORMALS[normalIndex];
	vec3 position = vec3(x, y, z);
	camDir = u_CamPos - position;
	fragPosSunViewSpace = u_SunViewProjectionMatrix * vec4(position.xyz, 1.0f);
	gl_Position =  u_Projection * u_View * vec4(position.xyz, 1.0f);
};

#shader fragment
#version 330 core

in vec3 normal;
in vec3 camDir;
in vec4 fragPosSunViewSpace;

layout(location = 0) out vec4 color;

const float minAmbientValue = 0.10;
const float maxAmbientValue = 0.15;
const float diffuseStrenght = 0.7;
const float global_illuminationStrenght = 0.1;
const float specularStrength = 10.0f;
const float fogCoefficient = 7.0f / 3.0f;
const float MaxFog = 1;

uniform vec3 u_lightDir;
uniform float u_ChunkDistance;
uniform sampler2D u_SunShadowTexture;

float calcShadow(float dotLightNormal)
{
	vec3 pos = fragPosSunViewSpace.xyz * 0.5f + 0.5f;

	if(pos.x < 0 || pos.x > 1 || pos.y < 0 || pos.y > 1)
	{
		return 1;
	}

    float bias = max(0.0002f * (1.0f - dotLightNormal), 0.00018f);
    
	float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(renderedTexture, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float depth = texture(renderedTexture, pos.xy + vec2(x, y) * texelSize).r;
            shadow += depth + bias < pos.z ? 0.0f : 1.0f;
        }
    }
    return shadow / 9.0f;
}

float fogDiv = u_ChunkDistance * fogCoefficient;

vec3 applyFog( vec3  rgb,       // original color of the pixel
               float distance ) // camera to point distance
{
    float fogAmount = min(exp((distance - (u_ChunkDistance - 3) * 16) / fogDiv), MaxFog);
    vec3  fogColor  = vec3(0.611f, 0.780f, 1.0f);
    return mix( rgb, fogColor, fogAmount );
}

void main()
{
	vec3 baseColor = vec3(0.2, 0.7, 0.05);
	vec3 sunColor = vec3(0.8, 0.8, 0.0);
	vec3 moonLightColor = vec3(0.02, 0.02, 0.07);

	vec3 lightDir = u_lightDir;
	vec3 moonLightDir = -u_lightDir;

	float sunsetEffect = max(sin(lightDir.y * 1.6f), 0);
	float moonEffect = max(sin(moonLightDir.y * 1.6f), 0);

	vec3 CurrentLightColor = (sunColor * float((lightDir.y > 0))) + (moonLightColor * float((moonLightDir.y > 0)));
	vec3 CurrentLight = CurrentLightColor * (sunsetEffect + moonEffect);

	vec3 ambient = max(maxAmbientValue * CurrentLight * length(normal), minAmbientValue * baseColor * length(normal));
	vec3 global_illumination = CurrentLight * global_illuminationStrenght;
	
	float sunDiff = max(dot(normal, lightDir), 0);
	float moonDiff = max(dot(normal, moonLightDir), 0);
	vec3 diffuse = diffuseStrenght * (sunDiff + moonDiff) * CurrentLight;

	float sunSpec     = pow(max(dot(normalize(normal), normalize(lightDir + normalize(camDir))), 0), 360);
	float moonSpec    = pow(max(dot(normalize(normal), normalize(moonLightDir + normalize(camDir))), 0), 360);
	//max prevents from having infinitly high specs.. also lets you have a standard spec image while on contact with the ground//
	vec3 specular     = CurrentLight * (specularStrength / max(length(camDir), 10)) * (sunSpec);
	 
	float shadow = calcShadow(dot(normal, lightDir));
	vec3 originalColor = baseColor * (ambient + global_illumination + diffuse * shadow) + specular * shadow;
	vec3 foggedColor = applyFog(originalColor, length(camDir));

	color = vec4(foggedColor, 1.0f);
};