#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 texCoords;

out vec2 TexCoords;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 mvpMatrix;

void main() {
	gl_Position = mvpMatrix * vec4(vertexPos, 1.0);
    TexCoords = texCoords;
}