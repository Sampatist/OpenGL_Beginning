#shader vertex
#version 330 core

layout(location = 0) in int data;

out vec3 normal;
out vec3 camDir;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform ivec2 u_ChunkOffset;
uniform vec3 u_CamPos;

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
	gl_Position =  u_Projection * u_View * u_Model * vec4(position.xyz, 1.0f);
};

#shader fragment
#version 330 core

in vec3 normal;
in vec3 camDir;
layout(location = 0) out vec4 color;

const float minAmbientValue = 0.10;
const float maxAmbientValue = 0.15;
const float diffuseStrenght = 0.7;
const float global_illuminationStrenght = 0.1;
const float specularStrength = 1f;
const float fogCoefficient = 7.0f / 3.0f;
const float MaxFog = 1;

uniform float u_GameTime;
uniform float u_ChunkDistance;

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

	vec3 lightDir = normalize(vec3(sin(u_GameTime),cos(u_GameTime),sin(u_GameTime)*0.4));
	vec3 moonLightDir = -lightDir;

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
	vec3 specular     = CurrentLight * (specularStrength / max(length(camDir), 10)) * (sunSpec + moonSpec);
	 
	vec3 originalColor = baseColor * (ambient + global_illumination + diffuse) + specular;
	vec3 foggedColor = applyFog(originalColor, length(camDir));
	color = vec4(foggedColor, 1.0f);
};