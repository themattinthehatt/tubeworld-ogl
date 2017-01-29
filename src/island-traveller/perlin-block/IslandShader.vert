#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 centerHeight;

out vec3 fragPos;
out vec2 texCoords;
out vec3 normal;

// values that stay constant for the whole mesh
uniform mat4 vpMatrix;

void main() // main function
{
    vec3 fragPos = centerHeight.xyz + vertexPos;
    fragPos = vec3(fragPos.x, fragPos.y, fragPos.z * centerHeight.w);
    gl_Position = vpMatrix
                * vec4(fragPos, 1);

    texCoords = vertexUV;
    normal = vertexNormal;
}