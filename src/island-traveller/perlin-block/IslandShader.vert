#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;

// values that stay constant for the whole mesh
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;

void main() // main function
{
    gl_Position = vpMatrix * mMatrix * vec4(vertexPos, 1);
}