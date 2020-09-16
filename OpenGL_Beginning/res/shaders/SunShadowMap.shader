#shader vertex
#version 400 core

layout(location = 0) in int data;

uniform mat4 u_SunViewProjectionMatrix;
uniform ivec2 u_ChunkOffset;

void main()
{
	int x = (data & 0x1F) + u_ChunkOffset.x;
	int z = ((data >> 5) & 0x1F) + u_ChunkOffset.y;
	int y = ((data >> 10) & 0x1FF);
	vec3 position = vec3(x, y, z);
	gl_Position =  u_SunViewProjectionMatrix * vec4(position.xyz, 1.0f);
};

#shader fragment
#version 330 core

void main()
{

};