#version 330 core
#define MAX_LIGHTS 100

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
    vec3 colorAmbient;
    vec3 colorDiffuse;
    vec3 colorSpecular;
};

struct Light {
    // Position is direction for dirlights
    vec4 position; // w is type (0 = DirLight, 1 = PointLight)
    vec4 ambient;  // w is constant term
    vec4 diffuse;  // w is linear term
    vec4 specular; // w is quadratic term
};

in vec2 texCoord;
in vec3 normal;
in vec3 fragmentPos;

out vec4 FragColor;

uniform Material material;

layout (std140) uniform Matricies {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

layout (std140) uniform Lights {
    uniform Light lights[MAX_LIGHTS];
    uint numActiveLights;
};

vec3 CalcDirLight(Light light, vec3 norm, vec3 viewDir)
{
    vec4 tex = texture(material.diffuse, texCoord);
    if (tex.a < 0.1)
	discard;

    // Ambient
    vec3 ambient = vec3(light.ambient) * material.colorAmbient * vec3(tex);

    // Diffuse
    vec3 lightDir = normalize(vec3(-light.position));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(light.diffuse) * material.colorDiffuse * vec3(texture(material.diffuse, texCoord));

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(light.specular) * material.colorSpecular * vec3(texture(material.specular, texCoord));

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(Light light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec4 tex = texture(material.diffuse, texCoord);
    if (tex.a < 0.1)
	discard;

    // Ambient
    vec3 ambient = vec3(light.ambient) * material.colorAmbient * vec3(tex);

    // Diffuse
    vec3 lightDir = normalize(vec3(light.position) - fragmentPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(light.diffuse) * material.colorDiffuse * vec3(texture(material.diffuse, texCoord));

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(light.specular) * material.colorSpecular * vec3(texture(material.specular, texCoord));

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

void main()
{
    vec3 norm = normalize(normal);

    vec3 viewDir = normalize(viewPos - fragmentPos);

    vec3 result = vec3(0.0f);

    for (uint i = 0u; i < numActiveLights; i++)
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
}
