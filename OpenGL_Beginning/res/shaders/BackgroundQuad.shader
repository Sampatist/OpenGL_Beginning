#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

out vec3 viewRay;

uniform mat4 u_projMatrix;
uniform mat4 u_viewMatrix;

void main()
{
	vec4 viewSpaceCoord = inverse(u_projMatrix) * vec4(position.xy, -1.0f, 1.0f);
	viewSpaceCoord = vec4(viewSpaceCoord.xy, -1.0f, 0.0f);
	viewRay = (inverse(u_viewMatrix) * viewSpaceCoord).xyz;
	gl_Position = vec4(position, 1.0f);
};

#shader fragment
#version 330 core

in vec3 viewRay;

layout(location = 0) out vec4 color;

uniform vec3 u_CamDir;

void main()
{
	vec3 nViewRay = normalize(viewRay);
	color = vec4(0.611f, 0.780f, 1.0f, 1.0f) * (1 - nViewRay.y * 0.8);
};