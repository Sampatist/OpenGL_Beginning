#shader vertex
#version 400 core

layout(location = 0) in vec3 position;

out vec3 viewRay;
out vec3 rotatedSkyBoxSampleVector;

uniform mat4 u_projMatrix;
uniform mat4 u_viewMatrix;
uniform vec3 u_lightDir;
uniform vec3 u_SunBinormal;

void main()
{
	vec4 viewSpaceCoord = inverse(u_projMatrix) * vec4(position.xy, -1.0f, 1.0f);
	viewSpaceCoord = vec4(viewSpaceCoord.xy, -1.0f, 0.0f);
	viewRay = vec3((inverse(u_viewMatrix) * viewSpaceCoord).xyz);
	vec3 nViewRay = normalize(viewRay);

	float theta = float(u_lightDir.x < 0) * 2 * acos(-1.0f) + acos(dot(u_lightDir, vec3(0,1,0))) * (float(u_lightDir.x < 0) * -2.0f + 1.0f);

	rotatedSkyBoxSampleVector = nViewRay * cos(theta) + cross(-u_SunBinormal, nViewRay) * sin(theta) + u_SunBinormal * (dot(u_SunBinormal, nViewRay) * (1- cos(theta)));

	gl_Position = vec4(position, 1.0f);
};

#shader fragment
#version 400 core

in vec3 viewRay;
in vec3 rotatedSkyBoxSampleVector;

layout(location = 0) out vec4 color;

uniform vec3 u_lightDir;
uniform vec3 u_CamPos;
uniform sampler2D u_gColor;
uniform sampler2D u_gDepth;
uniform samplerCube u_StarTexture;
uniform vec3 u_scatteringCoefficients;

const float PI = 3.1418;
const int numInScatteringPoints = 10;
const int numOpticalDepthPoints = 10;
const float atmosphereRadius = 10400;
const float planetRadius = 9000;
const float densityFalloff = 7;
vec3 planetCentre = vec3(0);
vec3 dirToSun = vec3(0);
const float zNear = 0.1f;
const float zFar = 900.0f;

const float minColorStars = 0.1f;

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

	//texColor += texture(u_StarTexture, rotatedSkyBoxSampleVector) * float(z_e > 900) * 
	//	clamp((0.1f - u_lightDir.y), 0.0f, 1.0f);
	//clamp((0.2f - u_lightDir.y) * (0.5f - pow(dot(nViewRay, u_lightDir), 3) * 0.5f), 0.0f, 1.0f);  Makes a little bit more sense
	//clamp((0.1f - u_lightDir.y), 0.0f, 1.0f);  Makes sense
	
	vec3 m = nViewRay - u_lightDir;
	vec3 d = m * 2000 + u_lightDir;
	vec3 d2 = m * 50 + u_lightDir;
	vec4 sun = max(pow((dot(d, u_lightDir)), 4), 0) * 3 * vec4(0.8, 0.8, 0.4, 1.0f) * float(z_e > 900);
	vec4 sunOuter = max(pow((dot(d2, u_lightDir)), 8), 0) * vec4(0.8, 0.8, 0.4, 1.0f) / 4 * float(z_e > 900);
	dirToSun = u_lightDir;

	//ATHMOSPHERE START /-/-/-/-/-/-/

	vec3 rayOrigin = u_CamPos;
	planetCentre = vec3(u_CamPos.x, -10000, u_CamPos.z);
	vec2 hitInfo = raySphere(planetCentre, atmosphereRadius, rayOrigin, nViewRay);
	
	float dstToAtmosphere = hitInfo.x;
	float dstThroughAtmosphere = min(hitInfo.y, z_e - dstToAtmosphere);

	vec4 finalColor;

	if (dstThroughAtmosphere > 0)
	{
		vec3 pointInAtmosphere = rayOrigin + nViewRay * dstToAtmosphere;
		vec3 light = calculateLight(pointInAtmosphere, nViewRay, dstThroughAtmosphere, texColor.xyz);
		finalColor = vec4(light, 0) + sun + sunOuter;
		
	
		//finalColor += texture(u_StarTexture, rotatedSkyBoxSampleVector) * float(z_e > 900) * clamp((0.05f - u_lightDir.y), 0.0f, 1.0f); even this works great
	}
	else
	{
		finalColor = texColor + sun + sunOuter;
	}

	//ATHMOSPHERE END /-/-/-/-/-/-/
	//finalColor = texColor + sun + sunOuter;


	//efsane bu yöntem... performans açýsýndan da iyi
	if(texture(u_gColor, texCoord) == 0)
		finalColor += texture(u_StarTexture, rotatedSkyBoxSampleVector)  * clamp(1 - (length(finalColor) * (1.0f/minColorStars)), 0.0f, 1.0f); // looks great but still problematic since it also works for blocks I mean it wont be a problem since there is ambient light everywhere but it is edgy
	//Big or small braaaiiinn???
	//finalColor += texture(u_StarTexture, rotatedSkyBoxSampleVector) * float(z_e > 900) * float(length(finalColor) < 0.1f); wowwwww looks lame as fuck 
	//Lets try again
	//finalColor += texture(u_StarTexture, rotatedSkyBoxSampleVector) * float(z_e > 900) * float(length(finalColor) < 0.1f) * clamp((0.1f - u_lightDir.y), 0.0f, 1.0f); kinda worked but still weird
	//finalColor += texture(u_StarTexture, rotatedSkyBoxSampleVector) * float(z_e > 900) * float(length(finalColor) < 0.1f) * clamp((0.05f- u_lightDir.y), 0.0f, 1.0f); lowering starting point made it better but I think thats not the way
	//finalColor += texture(u_StarTexture, rotatedSkyBoxSampleVector)  * clamp((0.05f - u_lightDir.y), 0.0f, 1.0f); hmm this is risky ....
	//finalColor += texture(u_StarTexture, rotatedSkyBoxSampleVector) * float(z_e > 900) * (float(length(finalColor) < 0.01f) + float(length(finalColor) < 0.025f) + float(length(finalColor) < 0.05f) + float(length(finalColor) < 0.1f)) / 4 * clamp((0.05f - u_lightDir.y), 0.0f, 1.0f); this really smoothened out it by a lot
	
	//best one for now
	//finalColor += texture(u_StarTexture, rotatedSkyBoxSampleVector) * float(z_e > 900) * (float(length(finalColor) < 0.01f) + float(length(finalColor) < 0.02f) + float(length(finalColor) < 0.03f) + float(length(finalColor) < 0.04f) + float(length(finalColor) < 0.05f) + float(length(finalColor) < 0.06f) + float(length(finalColor) < 0.07f) + float(length(finalColor) < 0.08f) + float(length(finalColor) < 0.09f) + float(length(finalColor) < 0.1f)) / 10 * clamp((0.05f - u_lightDir.y), 0.0f, 1.0f); // looks great but still problematic since it also works for blocks I mean it wont be a problem since there is ambient light everywhere but it is edgy
	


	//CrossHair
	vec2 W = vec2(1600, 900);
	if ((gl_FragCoord.x > W.x/2 - 3 && gl_FragCoord.x < W.x/2 + 3 && gl_FragCoord.y > W.y/2 - 14 && gl_FragCoord.y < W.y / 2 + 14) ^^
		(gl_FragCoord.x > W.x/2 - 14 && gl_FragCoord.x < W.x/2 + 14 && gl_FragCoord.y > W.y / 2 - 3 && gl_FragCoord.y < W.y / 2 + 3))
		finalColor = vec4(1.0f) - finalColor;

	color = finalColor;
};