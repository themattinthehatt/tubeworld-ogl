#version 330 core

// TextureGenerator class handles this data
struct Material {
    vec3 ambient;       // ambient light color of material (RGB)
    vec3 diffuse;       // diffuse light color of material (RGB)
    vec3 specular;      // specular light color of material (RGB)
    float alpha;        // alpha value
    float shininess;    // defines radius of specular highlight
    sampler2D loadedTexture;
};

// TubeLighting class handles this data
struct DirLight {
    vec3 direction;     // direction pointing away from the light

    vec3 ambient;       // ambient light color (RGB)
    vec3 diffuse;       // diffuse light color (RGB)
    vec3 specular;      // specular light color (RGB)
};

struct PointLight {
    vec3 position;      // position of light in world-space coordinates

    vec3 ambient;       // ambient light color (RGB)
    vec3 diffuse;       // diffuse light color (RGB)
    vec3 specular;      // specular light color (RGB)

    float lin;          // linear coefficient in attenuation calculation
    float quad;         // quadratic coefficient in attenuation calculation
};

struct SpotLight {
    vec3 position;      // position of light in world-space coordinates
    vec3 direction;     // direction pointing away from the light
    float innerTheta;   // specifies a spotlight's inner radius (cos of angle)
    float outerTheta;   // specifies a spotlight's outer radius (cos of angle)

    vec3 ambient;       // ambient light color (RGB)
    vec3 diffuse;       // diffuse light color (RGB)
    vec3 specular;      // specular light color (RGB)

    float lin;          // linear coefficient in attenuation calculation
    float quad;         // quadratic coefficient in attenuation calculation
};

// interpolated values from the vertex shaders
in vec3 fragmentPosition;
in vec2 textureCoords;
in vec3 normal;

// output data
out vec4 color;

// values that stay constant for the whole mesh
uniform vec3 cameraPosition;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

// forward declarations
vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 viewDir,
                    vec3 fragmentPosition);
vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 viewDir);



void main() {

    vec3 viewDir = normalize(cameraPosition - fragmentPosition);
    vec3 norm = normalize(normal);

//    result += calcDirLight(dirLight, norm, viewDir);
    vec3 result = calcPointLight(pointLight, norm, viewDir, fragmentPosition);
//    result += calcSpotLight(spotLight, norm, viewDir);

    // output color
    color = vec4(result, material.alpha);

}



vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {

    // calculate ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.loadedTexture, textureCoords));

    // calculate diffuse lighting
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.loadedTexture, textureCoords));

    // calculate specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(1.0f); // full spec highlights for now

    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normalVec, vec3 viewDir, vec3 fragPos) {

    // calculate ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.loadedTexture, textureCoords));

    // calculate diffuse lighting
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normalVec, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.loadedTexture, textureCoords));

    // calculate specular lighting
    vec3 reflectDir = reflect(-lightDir, normalVec);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); //material.shininess);
    vec3 specular = light.specular * spec * vec3(1.0f); // full spec highlights for now

    // calculate attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (1.0f + light.lin * distance +
                        light.quad * distance * distance);

    return ((ambient + diffuse + specular) * attenuation);
}

//vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 viewDir) {
//    // calculate ambient lighting
//    vec3 ambient = light.ambient * hsvToRgb(material.ambient);
//
//    // calculate other lighting effects if applicable
//    vec3 lightDir = normalize(light.position - fragmentPosition);
//    float angle = dot(-lightDir, light.direction);
//    float epsilon = light.innerTheta - light.outerTheta;
//    float intensity = clamp((angle - light.outerTheta) / epsilon, 0.0, 1.0);
//
//    // calculate diffuse lighting
//    vec3 norm = normalize(normal);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = light.diffuse * diff * hsvToRgb(material.diffuse);
//
//    // calculate specular lighting
//    vec3 viewDir = normalize(cameraPosition - fragmentPosition);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular = light.specular * spec * hsvToRgb(material.specular);
//
//    // calculate attenuation
//    float distance = length(light.position - fragmentPosition);
//    float attenuation = 1.0f / (1.0f + light.lin * distance +
//                                light.quad * distance * distance);
//    ambient *= attenuation; // leave ambient lighting out of spotlight calculations
//    diffuse *= attenuation * intensity;
//    specular *= attenuation * intensity;
//
//    // output color (leave ambient lighting out of spotlight calculations)
//    return (ambient  * attenuation
//           + diffuse  * attenuation * intensity
//           + specular * attenuation * intensity);
//
//}