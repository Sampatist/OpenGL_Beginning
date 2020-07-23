#shader vertex
#version 330 core

layout(location = 0) in int data;

out vec3 normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform ivec2 u_ChunkOffset;

const vec3 NORMALS[6] = vec3[6](
	vec3(-1.0f,0.0f,0.0f),
	vec3(1.0f,0.0f,0.0f),
	vec3(0.0f,0.0f,-1.0f),
	vec3(0.0f,0.0f,1.0f),
	vec3(0.0f,-1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f)
);

void main()
{
	int x = (data & 0xF) + u_ChunkOffset.x;
	int z = ((data >> 4) & 0xF) + u_ChunkOffset.y;
	int y = ((data >> 8) & 0xFF);
	int normalIndex = ((data >> 24) & 0x7);
	normal = NORMALS[normalIndex];
	vec3 position = vec3(x, y, z);
	gl_Position =  u_Projection * u_View * u_Model * vec4(position.xyz, 1.0f);
};

#shader fragment
#version 330 core

in vec3 normal;
layout(location = 0) out vec4 color;

void main()
{
   color = vec4(abs(normal),1.0f);
};