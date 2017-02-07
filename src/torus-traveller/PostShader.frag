#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;
uniform vec3 targetColor;
uniform float fadeStep;
uniform float fadeTotal;

void main() {

    const float exposure = 10.f;

    // sample from offscreen framebuffer
	vec3 Color = texture(screenTexture, TexCoords).rgb;

	// (Reinhard) tone mapping to transform hdr lighting to ldr lighting
	vec3 mappedColor = Color / (Color + vec3(1.f));

	// Exposure tone mapping
//	vec3 mappedColor = vec3(1.f) - exp(-Color * exposure);

	// fade
	color = vec4(mappedColor.x + (targetColor.x - mappedColor.x)*fadeStep/fadeTotal,
	             mappedColor.y + (targetColor.y - mappedColor.y)*fadeStep/fadeTotal,
	             mappedColor.z + (targetColor.z - mappedColor.z)*fadeStep/fadeTotal,
	             1.f);
}