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

const float specularStrength = 20;
const float ambientValue = 0.15;
const float global_illuminationStrenght = 0.17;
const float fogCoefficient = 7.0f / 3.0f;

uniform float u_GameTime;
uniform float u_ChunkDistance;

float fogDiv = u_ChunkDistance * fogCoefficient;

vec3 applyFog( vec3  rgb,       // original color of the pixel
               float distance ) // camera to point distance
{
    float fogAmount = min(exp((distance - (u_ChunkDistance - 2) * 16) / fogDiv), 1);
    vec3  fogColor  = vec3(0.611f, 0.780f, 1.0f);
    return mix( rgb, fogColor, fogAmount );
}

void main()
{
	vec3 baseColor = vec3(0.2, 0.7, 0.3);
	vec3 sunColor = vec3(0.8, 0.8, 0.4);
	vec3 moonLightColor = vec3(0.2, 0.2, 0.4);
	vec3 lightDir = normalize(vec3(sin(u_GameTime),cos(u_GameTime),sin(u_GameTime)*0.4));
	vec3 moonLightDir = -lightDir;

	float sunsetEffect = max(sin(lightDir.y * 1.6f), 0);
	float moonEffect = max(sin(moonLightDir.y * 1.6f), 0);

	float sunDiff = max(dot(normal, lightDir), 0);
	float moonDiff = max(dot(normal, moonLightDir), 0);
	vec3 sunDiffuse = sunDiff * sunsetEffect * sunColor * baseColor;
	vec3 moonDiffuse = moonDiff * moonEffect * moonLightColor * baseColor;
	vec3 diffuse = sunDiffuse + moonDiffuse;

	float sunSpec     = pow(max(dot(normal, normalize(lightDir + normalize(camDir))), 0), 16);
	float moonSpec    = pow(max(dot(normal, normalize(moonLightDir + normalize(camDir))), 0), 16);
	vec3 sunSpecular  = sunColor       * sunSpec  * specularStrength * sunsetEffect;
	vec3 moonSpecular = moonLightColor * moonSpec * specularStrength * moonEffect;
	vec3 specular     = sunSpecular + moonSpecular;

	vec3 global_illumination = baseColor * global_illuminationStrenght * sunsetEffect;
	vec3 ambient = baseColor * length(normal) * ambientValue;

	vec3 originalColor = ambient + diffuse + global_illumination + specular;
	vec3 foggedColor = applyFog(originalColor, length(camDir));
	color = vec4(foggedColor, 1.0f);
};