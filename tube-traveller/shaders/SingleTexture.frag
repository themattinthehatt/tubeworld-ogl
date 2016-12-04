#version 330 core

// interpolated values from the vertex shaders
in vec3 fragmentColor;
in vec2 textureCoords;

// output data
out vec4 color;

// values that stay constant for the whole mesh
uniform sampler2D loadedTexture;

void main() {
    // output color = color specified in the vertex shader,
    // interpolated between all 3 surrounding vertices
    color = texture(loadedTexture, textureCoords);
}