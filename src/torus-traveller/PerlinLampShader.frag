#version 330 core

// uniforms
uniform float threshold;
uniform vec3 lampColor;
uniform sampler2D loadedTexture;

// input data
in vec2 texCoords;

// output data
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main() {

    // get sampled texture value for lamp
    fragColor = texture(loadedTexture, texCoords);

    // only use texture value if it crosses a threshold
    if (fragColor.r+fragColor.g+fragColor.b > 3.f*threshold) {
        fragColor = vec4(lampColor, 1.f);
    } else {
        fragColor = vec4(0.f, 0.f, 0.f, 1.f);
    }

    // output values that cross a different threshold separately for bloom
    float brighness = dot(fragColor.rgb, vec3(0.33f, 0.33f, 0.33f)); //vec3(0.2126, 0.7152, 0.0722));
    if (brighness > 1.0) {
        brightColor = vec4(fragColor.rgb, 1.0);
    }

}