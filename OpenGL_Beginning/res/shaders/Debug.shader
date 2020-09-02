#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 out_color;

void main()
{
	out_color = color;
	gl_Position = u_Projection * u_View * vec4(pos, 1.0f);
};

#shader fragment
#version 330 core

in vec3 out_color;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(out_color, 1.0f);
};