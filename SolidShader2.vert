#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;

// output data
out vec3 fragmentColor;

// values that stay constant for the whole mesh
uniform mat4 MVP;

void main() // main function
{
    // sets the vertex position to whatever was in the buffer
    gl_Position = MVP * vec4(vertexPos, 1);

    // the color of each vertex will depend on it's distance from the origin
    fragmentColor.r = 0.01f * sqrt(vertexPos.x*vertexPos.x +
                      vertexPos.y*vertexPos.y +
                      vertexPos.z*vertexPos.z);
    fragmentColor.g = 1.0f;
    fragmentColor.b = 1.0f;
}


