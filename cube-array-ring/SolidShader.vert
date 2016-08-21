#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 ringCenter;
layout(location = 2) in vec3 ringOffset;
layout(location = 3) in mat4 rotationMatrix; // in 3,4,5,6

// output data
out vec3 fragmentColor;
out float sinTime;

// values that stay constant for the whole mesh
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;
uniform float time;

float angle;
float sinDist;
float sinTime2;
float sinTime3;

void main() // main function
{
    // calculate angle
    angle = atan(ringCenter.y, ringCenter.x);
//    sinDist = 2.0f * (sin(0.05f * dist + 0.10f) / 2.0f + 0.5f) + 1.0f;
//    sinTime = sin(time) / 2.0f + 0.5f;
//    sinTime2 = (sin(10f * (time - vertexCenter.y)) / 2.0f + 0.5f) + 1.0f;
//    // sets the vertex position to whatever was in the buffer
//    gl_Position = mvpMatrix * vec4(sinDist * sinTime2 * vertexPos + vertexCenter, 1);

    //sinDist = 2.0f * (sin(0.05f * dist + 0.10f) / 2.0f + 0.5f) + 0.5f;
    sinTime = time; //sin(0.5 * time) / 2.0f + 0.5.f;
    sinTime2 = (sin(5.0f * (time - angle)) / 2.0f + 0.5f) + 0.5f;
    sinTime3 = (sin(5.0f * (time - angle) + 3.14159f/2) / 2.0f + 0.5f) + 0.5f;
    gl_Position = vpMatrix * vec4(ringCenter +
                                  vec3(rotationMatrix *
                                  vec4(ringOffset +
                                  (sinTime2 + sinTime3) * vertexPos, 1)), 1);

    // the color of each vertex will depend on its distance from the origin
    fragmentColor.r = 2.f * angle / 3.14159f / 2;
    fragmentColor.g = 1.0f;
    fragmentColor.b = 1.0f;
}