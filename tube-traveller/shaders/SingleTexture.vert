#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 center;
layout(location = 2) in vec2 uvs;
layout(location = 3) in mat4 rotationMatrix; // in 3,4,5,6

// output data
out vec3 fragmentColor;
out vec2 textureCoords;

// values that stay constant for the whole mesh
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;
uniform float time;

float dist;
float sinDist;

void main() // main function
{

    // calculate distance from origin
//    dist = sqrt(ringCenter.x * ringCenter.x +
//                ringCenter.y * ringCenter.y +
//                ringCenter.z * ringCenter.z);
//
//    sinDist = 2.0f * (sin(0.01f * dist + 0.10f) / 2.0f + 0.5f) + 0.5f;

    gl_Position = vpMatrix * vec4(center +
                                  vec3(rotationMatrix *
                                  mMatrix * vec4(vertexPos, 1)), 1);

    // the color of each vertex will depend on its distance from the origin
    fragmentColor.r = 1.0f; //sinDist;
    fragmentColor.g = 1.0f;
    fragmentColor.b = 1.0f;

    textureCoords = uvs;
}
