#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 matrixViewProjection;
uniform vec3 viewPosition; //for fresnell effect

out vec2 TexCoords;
out vec3 toCameraVector; //for fresnell effect
out vec4 clipSpace;



const float tilingTextures = 8000.0;

void main()
{
	vec4 objectPositionInWorld = model * vec4(aPos, 1.0);
	clipSpace = matrixViewProjection * objectPositionInWorld;
	TexCoords = vec2(aPos.x, aPos.z) / tilingTextures;
    gl_Position = clipSpace;
	toCameraVector = viewPosition - (objectPositionInWorld).xyz;
}