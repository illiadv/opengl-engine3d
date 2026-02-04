#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};


in vec2 texCoord;
in vec3 normal;
// This in variable will be interpolated from the 3 world position vectors
// of the triangle to form the FragPos vector that is the per-fragment world position
in vec3 fragmentPos;
// in vec3 oLightPos;

out vec4 FragColor;

uniform vec3 viewPos;

uniform Material material;
void main()
{
    FragColor = vec4(1.0f, 0.4f, 0.0f, 1.0f);
}
