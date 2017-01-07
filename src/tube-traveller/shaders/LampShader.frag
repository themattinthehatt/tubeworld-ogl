#version 330 core

// output data
out vec3 color;

// values that stay constant for the whole mesh
uniform vec3 lampColor;

void main() {
    // output color = color specified in the vertex shader,
    // interpolated between all 3 surrounding vertices
    color = lampColor;
}
