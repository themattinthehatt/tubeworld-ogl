#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;

// values that stay constant for the whole mesh
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;

out vec2 texCoords;

void main() // main function
{
    gl_Position = mvpMatrix * vec4(vertexPos, 1);
    texCoords = vertexUV;
}