#version 330 core

// input vertex data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 center;
layout(location = 2) in vec2 uvs;
layout(location = 3) in mat4 rotationMatrix;  // in 3,4,5,6
layout(location = 7) in mat4 rotationMatrix2; // in 7,8,9,10

// output data
out vec2 textureCoords;

// values that stay constant for the whole mesh
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;

void main() // main function
{

//    gl_Position = vpMatrix * vec4(center +
//                  vec3(((0.5-vertexPos.x)*rotationMatrix2 +
//                       (0.5+vertexPos.x)*rotationMatrix) *
//                                  mMatrix * vec4(vertexPos, 1)), 1);
    gl_Position = vpMatrix * vec4(center +
                  vec3(((0.5-vertexPos.y)*rotationMatrix2 +
                       (0.5+vertexPos.y)*rotationMatrix) *
                                  mMatrix * vec4(vertexPos, 1)), 1);
    textureCoords = uvs;
}
