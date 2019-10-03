#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 matrixViewProjection;

void main()
{
    gl_Position = matrixViewProjection * model * vec4(aPos, 1.0);
}