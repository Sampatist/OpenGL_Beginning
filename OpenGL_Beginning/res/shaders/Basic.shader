#shader vertex
#version 330 core

layout(location = 0) in int data;

out vec3 normal;
out vec3 camDir;
out vec4 fragPosSunViewSpace;
flat out int id;
flat out int n;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform ivec2 u_ChunkOffset;
uniform vec3 u_CamPos;
uniform mat4 u_SunViewProjectionMatrix;

const vec3 NORMALS[6] = vec3[6](
	vec3(-0.7f,0.0f,0.0f),
	vec3(0.7f,0.0f,0.0f),
	vec3(0.0f,0.0f,-0.75f),
	vec3(0.0f,0.0f, 0.75f),
	vec3(0.0f,-0.8f,0.0f),
	vec3(0.0f,0.8f,0.0f)
);

void main()
{
	int x = (data & 0x1F) + u_ChunkOffset.x;
	int z = ((data >> 5) & 0x1F) + u_ChunkOffset.y;
	int y = ((data >> 10) & 0x1FF);
	id = ((data >> 19) & 0xFF);
	int normalIndex = ((data >> 27) & 0x7);
	n = normalIndex;
	normal = NORMALS[normalIndex];
	vec3 position = vec3(x, y, z);
	camDir = u_CamPos - position;
	fragPosSunViewSpace = u_SunViewProjectionMatrix * vec4(position.xyz, 1.0f);
	gl_Position = u_Projection * u_View * vec4(position.xyz, 1.0f);
};

#shader fragment
#version 330 core

in vec3 normal;
in vec3 camDir;
in vec4 fragPosSunViewSpace;
flat in int id;
flat in int n;

layout(location = 0) out vec4 color;

const float minAmbientValue = 0.05;
const float maxAmbientValue = 0.15;
const float diffuseStrenght = 0.6;
const float global_illuminationStrenght = 0.25;
const float specularStrengthCoef = 10.0f;
const float fogCoefficient = 7.0f / 3.0f;
const float MaxFog = 1;

uniform vec3 u_lightDir;
uniform vec3 u_lightDirForw;
uniform vec3 u_lightDirBackw;

uniform float u_ChunkDistance;
uniform sampler2DShadow u_SunShadowTexture;

const float PI = 3.1418;

float calcShadow(float dotLightNormal)
{
	vec3 pos = fragPosSunViewSpace.xyz * 0.5f + 0.5f;

	if(pos.x < 0 || pos.x > 1 || pos.y < 0 || pos.y > 1)
	{
		return 1;
	}

	float bias = 0.00005 * tan(acos(dotLightNormal));
	bias = max(min(bias, 0.0005), 0.000005);

    vec2 texelSize = 1.0f / textureSize(u_SunShadowTexture, 0);
    
	float shadow = 0;
	for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            shadow += texture(u_SunShadowTexture, vec3(pos.xy + vec2(x, y) * texelSize, pos.z - bias));
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
	//Setup
	vec3 baseColor = vec3(1.0, id / 2.0f, 1.00);
	if(id == 1)
	{
		bool is = (n != 5);
		baseColor = vec3(0.36, 0.88 - (0.5 * float(is)), 0.34);
	}
	else if(id == 2)
	{
		baseColor = vec3(0.2, 0.141, 0.139);
	}
	else if(id == 3)
	{
		baseColor = vec3(0.3, 0.32, 0.35);
	}
	else if (id == 4)
	{
		baseColor = vec3(0.3, 0.32, 0.75);
	}

	vec3 sunColor = vec3(1.0, 1.0, 0.75);
	vec3 moonLightColor = vec3(0.006, 0.02, 0.07);

	vec3 lightDir = u_lightDir;
	vec3 lightDirForw = u_lightDirForw;
	vec3 lightDirBackw = u_lightDirBackw;

	vec3 moonLightDir = -lightDir;

	float sunsetEffect = max(sin(lightDir.y * (PI/2)), 0);
	float sunsetEffectForw = max(sin(lightDirForw.y * (PI / 2)), 0);
	float sunsetEffectBackw = max(sin(lightDirBackw.y * (PI / 2)), 0);

	float moonEffect = max(sin(moonLightDir.y * (PI / 2)), 0);
	
	//vec3 CurrentLightColor = (sunColor * float((lightDir.y > 0)) + (moonLightColor * float(moonLightDir.y > 0)));
	//vec3 CurrentLight = CurrentLightColor * (sunsetEffect + moonEffect);

	////Ambient
	//sun
	vec3 ambientSunMiddle = max(maxAmbientValue * sunColor * sunsetEffect * length(normal), minAmbientValue * baseColor * length(normal));
	vec3 ambientSunForw = max(maxAmbientValue * sunColor * sunsetEffectForw * length(normal), minAmbientValue * baseColor * length(normal));
	vec3 ambientSunBackw = max(maxAmbientValue * sunColor * sunsetEffectBackw * length(normal), minAmbientValue * baseColor * length(normal));
	vec3 ambientSun = (ambientSunMiddle + ambientSunForw + ambientSunBackw) / 3;
	//moon
	vec3 ambientMoon = max(maxAmbientValue * moonLightColor * moonEffect * length(normal), minAmbientValue * baseColor * length(normal));
	///total
	vec3 ambient = ambientSun + ambientMoon;

	////Global Illumination
	//sun
	vec3 global_illuminationSunMiddle = sunColor * sunsetEffect * global_illuminationStrenght;
	vec3 global_illuminationSunForw = sunColor * sunsetEffectForw * global_illuminationStrenght;
	vec3 global_illuminationSunBackw = sunColor * sunsetEffectBackw * global_illuminationStrenght;
	vec3 global_illuminationSun = (global_illuminationSunMiddle + global_illuminationSunForw + global_illuminationSunBackw) / 3;
	//moon
	vec3 global_illuminationMoon = moonLightColor * moonEffect * global_illuminationStrenght;
	///total
	vec3 global_illumination = global_illuminationSun + global_illuminationMoon;


	////Diffuse
	float specularStrenght = (specularStrengthCoef / max(length(camDir), 10));
	float sunShadow = calcShadow(dot(normalize(normal), lightDir));
	//sun
	float sunDiffMiddle = max(dot(normal, lightDir), 0) * sunsetEffect * sunShadow;
	float sunDiffForw = max(dot(normal, lightDirForw), 0) * sunsetEffectForw * sunShadow;
	float sunDiffBackw = max(dot(normal, lightDirBackw), 0) * sunsetEffectBackw * sunShadow;
	float sunDiff = (sunDiffMiddle + sunDiffForw + sunDiffBackw)/3;
	vec3 sunDiffuse = diffuseStrenght * sunDiff * sunColor;
	//moon
	float moonDiff = max(dot(normal, moonLightDir), 0) * moonEffect;
	vec3 moonDiffuse = diffuseStrenght * moonDiff * moonLightColor;
	///total
	vec3 diffuse = sunDiffuse + moonDiffuse;
	
	////Specular
	float sunSpec = pow(max(dot(normalize(normal), normalize(lightDir + normalize(camDir))), 0), 360) * sunShadow;
	vec3 sunSpecular = specularStrenght * sunSpec * sunColor * sunsetEffect;
	
	float moonSpec    = pow(max(dot(normalize(normal), normalize(moonLightDir + normalize(camDir))), 0), 360);
	vec3 moonSpecular = specularStrenght * moonSpec * moonLightColor * moonEffect;
	
	vec3 specular = sunSpecular + moonSpecular;

	vec3 originalColor = baseColor * (ambient + global_illumination + diffuse) + specular;

	color = vec4(originalColor, 1.0f);
};