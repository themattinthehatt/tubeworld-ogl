#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 uvs;
layout(location = 3) in vec3 center;
layout(location = 4) in mat4 rotationMatrix;  // in 4,5,6,7
layout(location = 8) in mat4 rotationMatrix2; // in 8,9,10,11

// output data
out vec3 fragmentPosition;
out vec2 textureCoords;
out vec3 normal;

// values that stay constant for the whole mesh
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;

void main() // main function
{
    fragmentPosition = center + vec3(((0.5-vertexPos.y)*rotationMatrix2 +
                            (0.5+vertexPos.y)*rotationMatrix) *
                            mMatrix * vec4(vertexPos, 1.f));
    gl_Position = vpMatrix * vec4(fragmentPosition, 1);
    textureCoords = uvs;
    normal = ((0.5-vertexPos.y)*mat3(rotationMatrix2) +
              (0.5+vertexPos.y)*mat3(rotationMatrix)) *
              mat3(mMatrix) * normals;
}