#version 330 core

// interpolated values from the vertex shaders
//in vec3 fragmentColor;
in vec3 fragmentColor;
in float sinTime;

// output data
out vec3 color;

//
//uniform float time;

// forward declaration
vec3 hsvToRgb(vec3 hsv);

// values that stay constant for the whole mesh
//uniform float time;

void main() {
    // output color = color specified in the vertex shader,
    // interpolated between all 3 surrounding vertices
    color = fragmentColor;
    color.r += 0.3 * sinTime; // let GPU throw out numbers > 1.0
    color = hsvToRgb(color);
}

// Thanks to sam at http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
// (May 19, 2015).
const vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
vec3 hsvToRgb(vec3 c){
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}