#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;
uniform vec3 targetColor;
uniform float fadeStep;
uniform float fadeTotal;

void main() {
	vec4 Color = texture(screenTexture, TexCoords);
	color = vec4(Color.x + (targetColor.x - Color.x)*fadeStep/fadeTotal,
	             Color.y + (targetColor.y - Color.y)*fadeStep/fadeTotal,
	             Color.z + (targetColor.z - Color.z)*fadeStep/fadeTotal,
	             Color.w);
}