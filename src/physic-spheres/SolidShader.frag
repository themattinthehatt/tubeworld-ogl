#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float alpha;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// interpolated values from the vertex shaders
in vec3 normal;
in vec3 fragmentPosition;

// output data
out vec4 color;

// values that stay constant for the whole mesh
uniform vec3 cameraPosition;
uniform Material material;
uniform Light light;

// forward declaration
vec3 hsvToRgb(vec3 hsv);

void main() {

    // calculate ambient lighting
    vec3 ambient = light.ambient * hsvToRgb(material.ambient);

    // calculate diffuse lighting
    vec3 lightDir = normalize(light.position - fragmentPosition);
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * hsvToRgb(material.diffuse);

    // calculate specular lighting
    vec3 viewDir = normalize(cameraPosition - fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * hsvToRgb(material.specular);

    // output color
    color = vec4(ambient + diffuse + specular, material.alpha);

}

// Thanks to sam at http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
// (May 19, 2015).
const vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
vec3 hsvToRgb(vec3 c){
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}