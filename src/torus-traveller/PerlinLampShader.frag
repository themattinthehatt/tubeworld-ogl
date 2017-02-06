#version 330 core

// uniforms
uniform float threshold;
uniform vec3 lampColor;
uniform sampler2D loadedTexture;

// input data
in vec2 texCoords;

// output data
out vec4 color;

void main() {

    color = texture(loadedTexture, texCoords);
    if (color.x+color.y+color.z < 3*threshold) {
        color = vec4(0.f, 0.f, 0.f, 1.f);
    } else {
        color = vec4(lampColor, 1.f);
    }

}