#version 330 core
// in vec2 texCoord;
in vec3 normal;
// This in variable will be interpolated from the 3 world position vectors
// of the triangle to form the FragPos vector that is the per-fragment world position
// in vec3 fragmentPos;
// in vec3 oLightPos;
in vec3 LightColor;

out vec4 FragColor;
// uniform sampler2D ourTexture;
// uniform sampler2D ourTexture2;
// uniform float mixFactor;
// uniform vec3 lightColor;
uniform vec3 objectColor;
// uniform vec3 viewPos;

void main()
{
    FragColor = vec4(objectColor * LightColor, 1.0);
}
