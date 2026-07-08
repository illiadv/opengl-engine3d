#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (std140) uniform Matricies {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 model;
uniform mat3 normalMat;

out vec3 fragmentPos;
out vec3 normal;
out vec2 texCoord;

void main()
{
    texCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Fragment position in world  space
    fragmentPos = vec3(model * vec4(aPos, 1.0));

    normal = normalMat * aNormal;
}
