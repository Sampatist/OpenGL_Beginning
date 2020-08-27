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
uniform vec3 u_CamPos;
uniform sampler2D u_gColor;
uniform sampler2D u_gDepth;
uniform vec3 u_scatteringCoefficients;

const float PI = 3.1418;
const int numInScatteringPoints = 10;
const int numOpticalDepthPoints = 2;
const float atmosphereRadius = 10400;
const float planetRadius = 9390;
const float densityFalloff = 7;
vec3 planetCentre = vec3(0);
vec3 dirToSun = vec3(0);
const float zNear = 0.1f;
const float zFar = 900.0f;

vec2 raySphere(vec3 sphereCentre, float sphereRadius, vec3 rayOrigin, vec3 rayDir)
{
	vec3 offset = rayOrigin - sphereCentre;
	float a = 1;
	float b = 2 * dot(offset, rayDir);
	float c = dot(offset, offset) - sphereRadius * sphereRadius;
	float d = b * b - 4 * a * c;

	if (d > 0)
	{
		float s = sqrt(d);
		float dstToSphereNear = max(0, (-b - s) / (2 * a));
		float dstToSphereFar = (-b + s) / (2 * a);

		if (dstToSphereFar >= 0)
		{
			return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
		}
	}
	return vec2(999999, 0);
}

float densityAtPoint(vec3 densitySamplePoint)
{
	float heightAboveSurface = length(densitySamplePoint - planetCentre) - planetRadius;
	float height01 = heightAboveSurface / (atmosphereRadius - planetRadius);
	float localDensity = exp(-height01 * densityFalloff) * (1 - height01);
	return localDensity;
}

float opticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength)
{
	vec3 densitySamplePoint = rayOrigin;
	float stepSize = rayLength / (numOpticalDepthPoints - 1);
	float opticalDepth = 0;
	for (int i = 0; i < numOpticalDepthPoints; i++)
	{
		float localDensity = densityAtPoint(densitySamplePoint);
		opticalDepth += localDensity * stepSize;
		densitySamplePoint += rayDir * stepSize;
	}
	return opticalDepth;
}

vec3 calculateLight(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 originalCol)
{
	vec3 inScatterPoint = rayOrigin;
	float stepSize = rayLength / (numInScatteringPoints - 1);
	vec3 inScatteredLight = vec3(0);
	float viewRayOpticalDepth = 0;

	for (int i = 0; i < numInScatteringPoints; i++)
	{
		float sunRayLength = raySphere(planetCentre, atmosphereRadius, inScatterPoint, dirToSun).y;
		float sunRayOpticalDepth = opticalDepth(inScatterPoint, dirToSun, sunRayLength);
		viewRayOpticalDepth = opticalDepth(inScatterPoint, -rayDir, stepSize * i);
		vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * u_scatteringCoefficients);
		float localDensity = densityAtPoint(inScatterPoint);

		inScatteredLight += localDensity * transmittance * u_scatteringCoefficients * stepSize;
		inScatterPoint += rayDir * stepSize;
	}
	float originalColTransmittance = exp(-viewRayOpticalDepth);
	return originalCol * originalColTransmittance + inScatteredLight;
}

void main()
{
	vec3 nViewRay = normalize(viewRay);

	vec2 texCoord = vec2(gl_FragCoord.x / 1600, gl_FragCoord.y / 900);
	vec4 texColor = texture(u_gColor, texCoord);
	float z_b = texture(u_gDepth, texCoord).r;
	float z_n = 2.0 * z_b - 1.0;
	float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));

	vec3 m = nViewRay - u_lightDir;
	vec3 d = m * 2000 + u_lightDir;
	vec3 d2 = m * 50 + u_lightDir;
	vec4 sun = max(pow((dot(d, u_lightDir)), 4), 0) * 3 * vec4(0.8, 0.8, 0.4, 1.0f) * float(z_e > 900);
	vec4 sunOuter = max(pow((dot(d2, u_lightDir)), 8), 0) * vec4(0.8, 0.8, 0.4, 1.0f) / 4 * float(z_e > 900);
	dirToSun = u_lightDir;

	vec3 rayOrigin = u_CamPos;
	vec3 rayDir = normalize(viewRay);
	planetCentre = vec3(u_CamPos.x, -10000, u_CamPos.z);
	vec2 hitInfo = raySphere(planetCentre, atmosphereRadius, rayOrigin, rayDir);
	
	float dstToAtmosphere = hitInfo.x;
	float dstThroughAtmosphere = min(hitInfo.y, z_e - dstToAtmosphere);

	if (dstThroughAtmosphere > 0)
	{
		vec3 pointInAtmosphere = rayOrigin + rayDir * dstToAtmosphere;
		vec3 light = calculateLight(pointInAtmosphere, rayDir, dstThroughAtmosphere, texColor.xyz);
		color = vec4(light, 0) + sun + sunOuter;
	}
	else
	{
		color = texColor + sun + sunOuter;
	}
};