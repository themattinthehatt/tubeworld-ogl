#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;

// values that stay constant for the whole mesh
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;

void main() // main function
{
    vec3 center = vec3(2.f, 10.f, 2.f);
    gl_Position = vpMatrix * vec4(center + vertexPos, 1);
}
