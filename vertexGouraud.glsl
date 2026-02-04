#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// layout (location = 1) in vec2 aTexCoord;
// out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMat;
uniform vec3 lightPos;
uniform vec3 lightColor;

// out vec3 normal;
// out vec3 fragmentPos;
// out vec3 oLightPos;

out vec3 LightColor;

void main()
{
    // texCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // fragment position in world^W view space
    vec3 fragmentPos = vec3(view * model * vec4(aPos, 1.0));
    vec3 LightPos = vec3(view * vec4(lightPos, 1.0));
    vec3 normal = normalMat * aNormal;

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal); // normalize everything
    vec3 lightDir = normalize(LightPos - fragmentPos);

    float diffStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffStrength * lightColor;

    // specular

    float specularStrength = 1.0;
    vec3 viewDir = normalize(/*viewPos*/ - fragmentPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * lightColor * spec;

    vec3 result = (ambient + diffuse + specular) * lightColor;
    LightColor = result;

    // Inversing matrices is a costly operation for shaders, so wherever possible try to 
    // avoid doing inverse operations since they have to be done on each vertex of your scene. 
    // For learning purposes this is fine, but for an efficient application you'll likely want to 
    // calculate the normal matrix on the CPU and send it to the shaders via a uniform 
    // before drawing (just like the model matrix). 
    // normal = mat3(transpose(inverse(view * model))) * aNormal;
    // normal = aNormal;
}
