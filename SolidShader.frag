#version 330 core

// interpolated values from the vertex shaders
//in vec3 fragmentColor;
in vec3 fragmentColor;

// output data
out vec3 color;

// forward declaration
vec3 hsvToRgb(vec3 hsv);

// values that stay constant for the whole mesh
uniform float hueMultiplier;

void main() {
    // output color = color specified in the vertex shader,
    // interpolated between all 3 surrounding vertices
//    fragmentColor.r = hueMultiplier * fragmentColor.r;
    color = hsvToRgb(fragmentColor);
}

// Thanks to sam at http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
// (May 19, 2015).
const vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
vec3 hsvToRgb(vec3 c){
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    vec3 rgb = c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    return rgb;
}