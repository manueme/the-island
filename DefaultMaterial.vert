#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosShadowLightSpace;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 shadowLightSpaceMatrix;

uniform vec4 clippingPlane;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosShadowLightSpace = shadowLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	vec4 worldPosition = model * vec4(aPos, 1.0);
	gl_ClipDistance[0] = dot(worldPosition, clippingPlane);
    gl_Position = projection * view * worldPosition;
}