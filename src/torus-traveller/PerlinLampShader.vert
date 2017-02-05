#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 modelCenter;

// values that stay constant for the whole mesh
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;

void main() // main function
{
    gl_Position = vpMatrix * vec4(modelCenter + vertexPos, 1);
}