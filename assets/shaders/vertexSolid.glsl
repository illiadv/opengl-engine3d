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

out vec3 normal;
out vec3 fragmentPos;
out vec3 oLightPos;

void main()
{
    // texCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // fragment position in world^W view space
    fragmentPos = vec3(view * model * vec4(aPos, 1.0));
    oLightPos = vec3(view * vec4(lightPos, 1.0));

    // Inversing matrices is a costly operation for shaders, so wherever possible try to 
    // avoid doing inverse operations since they have to be done on each vertex of your scene. 
    // For learning purposes this is fine, but for an efficient application you'll likely want to 
    // calculate the normal matrix on the CPU and send it to the shaders via a uniform 
    // before drawing (just like the model matrix). 
    // normal = mat3(transpose(inverse(view * model))) * aNormal;
    normal = normalMat * aNormal;
    // normal = aNormal;
}
