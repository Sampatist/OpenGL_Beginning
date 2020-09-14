#shader vertex
#version 400 core

layout(location = 0) in int data;

uniform dmat4 u_SunViewProjectionMatrix;
uniform ivec2 u_ChunkOffset;

void main()
{
	int x = (data & 0x1F) + u_ChunkOffset.x;
	int z = ((data >> 5) & 0x1F) + u_ChunkOffset.y;
	int y = ((data >> 10) & 0x1FF);
	dvec3 position = dvec3(x, y, z);
	gl_Position =  vec4(u_SunViewProjectionMatrix * dvec4(position.xyz, 1.0f));
};

#shader fragment
#version 330 core

void main()
{

};