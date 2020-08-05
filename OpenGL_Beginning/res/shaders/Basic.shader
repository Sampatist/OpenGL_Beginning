#shader vertex
#version 330 core

layout(location = 0) in int data;

out vec3 normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform ivec2 u_ChunkOffset;

const vec3 NORMALS[6] = vec3[6](
	vec3(-0.7f,0.0f,0.0f),
	vec3(0.7f,0.0f,0.0f),
	vec3(0.0f,0.0f,-0.5f),
	vec3(0.0f,0.0f, 0.5f),
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
	gl_Position =  u_Projection * u_View * u_Model * vec4(position.xyz, 1.0f);
};

#shader fragment
#version 330 core

in vec3 normal;
layout(location = 0) out vec4 color;

const float ambient = 0.5f;

uniform float u_GameTime;

void main()
{
	vec3 light = vec3(sin(u_GameTime),cos(u_GameTime),sin(u_GameTime)*0.4);
	float lightAmount = max(dot(normal, light), 0) * sin(light.y * 1.6f);
	if(light.y < 0)
	{
		lightAmount = 0;
	}
	vec3 baseColor = vec3(0.2 + sin(u_GameTime) * 0.45f, 0.4 - cos(u_GameTime * 2) * 0.3, 0.5 + sin(u_GameTime * 5) * 0.2);
	color = vec4(min(baseColor * lightAmount + baseColor * length(normal) * ambient, 0.7f), 1.0f);
};