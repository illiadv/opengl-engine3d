#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    // vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// in vec2 texCoord;
in vec3 normal;
// This in variable will be interpolated from the 3 world position vectors
// of the triangle to form the FragPos vector that is the per-fragment world position
in vec3 fragmentPos;
in vec3 oLightPos;

out vec4 FragColor;
// uniform sampler2D ourTexture;
// uniform sampler2D ourTexture2;
// uniform float mixFactor;
// uniform vec3 lightColor;
uniform vec3 objectColor;
// uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main()
{
    // FragColor = mix(texture(ourTexture, texCoord), texture(ourTexture2, vec2(texCoord.x, -texCoord.y)), mixFactor);

    vec3 ambient = material.ambient * light.ambient;

    // diffuse
    vec3 norm = normalize(normal); // normalize everything
    vec3 lightDir = normalize(oLightPos - fragmentPos);

    float diffStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffStrength * light.diffuse * material.diffuse;

    // specular

    float specularStrength = 1.0;
    vec3 viewDir = normalize(/*viewPos*/ - fragmentPos);
    // Note that we negate the lightDir vector. The reflect function expects 
    // the first vector to point from the light source towards the fragment's 
    // position, but the lightDir vector is currently pointing the other way around
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * light.specular * spec;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}
