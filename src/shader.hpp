#ifndef SHADER_H
#define SHADER_H

// #include <GL/glew.h>
#include <glad/glad.h>

void SetMat4(unsigned int shader, const char* uniform, float* matrix);

void SetMat3(unsigned int shader, const char* uniform, float* matrix);

void SetVec3(unsigned int shader, const char* uniform, const float* vec3);

void SetVec3(unsigned int shader, const char* uniform, float x, float y, float z);

void SetVec4(unsigned int shader, const char* uniform, const float* vec4);

void SetFloat(unsigned int shader, const char* uniform, float f);

unsigned int CreateShader(const char* vertexShaderPath, const char* fragmentShaderPath);

#endif
