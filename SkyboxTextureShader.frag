#version 330 core

// direction vector representing a 3D texture coordinate
in vec3 textureCoords;

// output data
out vec4 color;

// values that stay constant for the whole mesh
uniform samplerCube skyboxSampler;

void main() {
    // output color = color of the texture at the specified UV
    color = texture(skyboxSampler, textureCoords);
}
