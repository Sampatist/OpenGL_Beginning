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

uniform vec3 u_lightDir;
uniform vec3 u_camPos;

const float PI = 3.1418;

void main()
{
	vec3 nViewRay = normalize(viewRay);
	vec3 m = nViewRay - u_lightDir;
	vec3 d = m * 2000 + u_lightDir;
	vec3 d2 = m * 50 + u_lightDir;
	vec4 sun = max(pow((dot(d, u_lightDir)), 4), 0) * 3 * vec4(0.8, 0.8, 0.4, 1.0f);
	vec4 sunOuter = max(pow((dot(d2, u_lightDir)), 8), 0) * vec4(0.8, 0.8, 0.4, 1.0f) / 4;

	// TODO: sunset / sunrise effect
	// fuck thish
	// sun rim
	// calculate plane normal
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	// project nViewRay onto plane
	vec3 projectedRay = nViewRay - (dot(nViewRay, up)) * up;
	projectedRay = normalize(projectedRay - vec3(0.0f,0.2f,0.0f));
	m = nViewRay - projectedRay;
	d = m * 5 + projectedRay;
	float rimStrength = pow(clamp(dot(projectedRay, d),0,1),2) * (dot(nViewRay,u_lightDir)/2+0.5f);
	// small brain
	//float rimTest = (float(u_lightDir.y < 0.005) * float(u_lightDir.y > -0.015) * (-u_lightDir.y+0.005+float(u_lightDir.y < -0.005) * 2 * (u_lightDir.y+0.005))*200);
	float sunsetRange = 0.3f;
	float rimTest = (float(u_lightDir.y < sunsetRange) * float(u_lightDir.y > -sunsetRange) * ( sin((u_lightDir.y + sunsetRange) * (PI/(2* sunsetRange)) )) );
	vec4 rim = rimStrength * vec4(0.8f, 0.2f, 0.2f, 1.0f) * rimTest;
	//      /                                SKY     Color                                        /  /sun/  / sun outer with control/  /ufuk/
	color = vec4(0.611f, 0.780f, 1.0f, 1.0f) * (1 - ((dot(nViewRay,-u_lightDir)/2 + 0.5f) * vec4(0.4f, 0.35f, 0.3f, 1.0f)) ) + sun + (sunOuter/ (rimTest*2+1)) + rim;
};