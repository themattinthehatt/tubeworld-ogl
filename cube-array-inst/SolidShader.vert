#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexCenter;

// output data
out vec3 fragmentColor;
out float sinTime;

// values that stay constant for the whole mesh
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;
uniform float time;

float dist;
float sinDist;
float sinTime2;
float sinTime3;

void main() // main function
{
    // calculate distance of vertex from origin
    dist = sqrt(vertexCenter.x * vertexCenter.x +
                vertexCenter.y * vertexCenter.y +
                vertexCenter.z * vertexCenter.z);

//    sinDist = 2.0f * (sin(0.05f * dist + 0.10f) / 2.0f + 0.5f) + 1.0f;
//    sinTime = sin(time) / 2.0f + 0.5f;
//    sinTime2 = (sin(10f * (time - vertexCenter.y)) / 2.0f + 0.5f) + 1.0f;
//    // sets the vertex position to whatever was in the buffer
//    gl_Position = mvpMatrix * vec4(sinDist * sinTime2 * vertexPos + vertexCenter, 1);

    sinDist = 2.0f * (sin(0.05f * dist + 0.10f) / 2.0f + 0.5f) + 0.5f;
    sinTime = sin(0.1 * time) / 2.0f + 0.5f;
    sinTime2 = (sin(5.0f * (time - vertexCenter.y)) / 2.0f + 0.5f) + 0.5f;
    sinTime3 = (sin(5.0f * (time - vertexCenter.y) + 3.14159f/2) / 2.0f + 0.5f) + 0.5f;
    // sets the vertex position to whatever was in the buffer
    gl_Position = vpMatrix * vec4((sinTime2 + sinTime3) * vertexPos + vertexCenter, 1);

    // the color of each vertex will depend on its distance from the origin
    fragmentColor.r = 0.01f * dist;
    fragmentColor.g = 1.0f;
    fragmentColor.b = 1.0f;
}


