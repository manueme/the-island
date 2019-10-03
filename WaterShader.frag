#version 330 core
out vec4 FragColor;

in vec4 clipSpace;
in vec2 TexCoords;
in vec3 toCameraVector;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D waterDistortionMap;

const float waveStrength = 0.005;

uniform float moveFactor;

void main()
{    
	//Fresnel
	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));

	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);
	
	vec2 distortedTexCoords = texture(waterDistortionMap, vec2(TexCoords.x + moveFactor, TexCoords.y)).rg*0.5;
	distortedTexCoords = TexCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(waterDistortionMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

	vec4 reflectColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColour = texture(refractionTexture, refractTexCoords);

	FragColor = mix(reflectColour, refractColour, refractiveFactor);
}