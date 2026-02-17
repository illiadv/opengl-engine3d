#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    // direction for dirlights
    vec4 position; // w is type (0 = DirLight, 1 = PointLight)
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct PointLight {
    vec3 position;

    // float constant;
    // float linear;
    // float quadratic;

    vec4 ambient; // w is constant
    vec4 diffuse; // w is linear
    vec4 specular; // w is quadratic
};

struct SpotLight {
    vec3 position;

    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
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

#define MAX_LIGHTS 100

uniform int numActiveLights;

layout (std140) uniform Lights {
    uniform Light lights[MAX_LIGHTS];
};


// sun
vec3 CalcDirLight(Light light, vec3 norm, vec3 viewDir)
{
    // ambient
    vec3 ambient = vec3(light.ambient) * vec3(texture(material.diffuse, texCoord));

    // diffuse
    vec3 lightDir = normalize(vec3(-light.position));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(light.diffuse) * vec3(texture(material.diffuse, texCoord));

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(light.specular) * vec3(texture(material.specular, texCoord));

    return ambient + diffuse + specular;
    // return specular;
}

vec3 CalcPointLight(Light light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 ambient = vec3(light.ambient) * vec3(texture(material.diffuse, texCoord));

    vec3 lightDir = normalize(vec3(light.position) - fragmentPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(light.diffuse) * vec3(texture(material.diffuse, texCoord));

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(light.specular) * vec3(texture(material.specular, texCoord));

    // attenuation
    float d = length(vec3(light.position) - fragmentPos);
    float constant = light.ambient.w;
    float linear = light.diffuse.w;
    float quadratic = light.specular.w;
    float attenuation = 1.0 / (constant + linear * d + quadratic * d*d);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

    vec3 lightDir = normalize(light.position - fragmentPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, texCoord));

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * vec3(texture(material.specular, texCoord));

    // attenuation
    float d = length(light.position - fragmentPos);
    float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * d*d);

    // spotilight
    float theta = dot(lightDir, normalize(-light.direction));
    // soft edge
    float epsilon = light.cutOff - light.outerCutOff;
    float cutOffIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);


    ambient *= attenuation;
    diffuse *= attenuation * cutOffIntensity;
    specular *= attenuation * cutOffIntensity;

    return ambient + diffuse + specular;
}

void main()
{
    // FragColor = mix(texture(ourTexture, texCoord), texture(ourTexture2, vec2(texCoord.x, -texCoord.y)), mixFactor);

    // vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

    // diffuse
    vec3 norm = normalize(normal); // normalize everything

    vec3 viewDir = normalize(viewPos - fragmentPos);

    // vec3 result = CalcDirLight(dirLight, norm, viewDir);
    vec3 result = vec3(0.0f);

	//    result += CalcSpotLight(spotLight, norm, fragmentPos, viewDir);

    for (int i = 0; i < numActiveLights; i++)
    {
	float type = lights[i].position.w;
	if (type > 0.1f) {
	    result += CalcPointLight(lights[i], norm, fragmentPos, viewDir);
	}
	else {
	    result += CalcDirLight(lights[i], norm, viewDir);
	}
    }

    FragColor = vec4(result, 1.0);
    // FragColor = vec4(norm, 1.0);
    // FragColor = vec4(texCoord, 0.0, 1.0);
}
