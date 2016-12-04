#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPosition; // will contain position of vertex

// output data
out vec3 textureCoords;

// values that stay constant for the whole mesh
uniform mat4 viewMatrix;

void main() // main function
{
    // sets the vertex position to whatever was in the buffer
    gl_Position = viewMatrix * vec4(vertexPosition, 1.0f);

    // the color of each vertex will be interpolated to produce
    // the color of each fragment
    textureCoords = vertexPosition;
}