#include "shader.hpp"
#include <stdio.h>
#include <fstream>
#include <string>


void SetMat4(unsigned int shader, const char* uniform, float* matrix)
{
    int location = glGetUniformLocation(shader, uniform);
    if (location == -1)
        {}//printf("Failed to get location of the uniform \"%s\"\n", uniform);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

void SetMat3(unsigned int shader, const char* uniform, float* matrix)
{
    int location = glGetUniformLocation(shader, uniform);
    if (location == -1)
        {}//printf("Failed to get location of the uniform \"%s\"\n", uniform);
    glUniformMatrix3fv(location, 1, GL_FALSE, matrix);
}

void SetVec3(unsigned int shader, const char* uniform, const float* vec3) {
    int location = glGetUniformLocation(shader, uniform);
    if (location == -1)
        {}//printf("Failed to get location of the uniform \"%s\"\n", uniform);
    glUniform3fv(location, 1, vec3);
}

void SetVec3(unsigned int shader, const char* uniform, float x, float y, float z) {
    int location = glGetUniformLocation(shader, uniform);
    if (location == -1)
        {}//printf("Failed to get location of the uniform \"%s\"\n", uniform);
    glUniform3f(location, x, y, z);
}

void SetVec4(unsigned int shader, const char* uniform, const float* vec4) {
    int location = glGetUniformLocation(shader, uniform);
    if (location == -1)
        {}//printf("Failed to get location of the uniform \"%s\"\n", uniform);
    glUniform4fv(location, 1, vec4);
}

void SetFloat(unsigned int shader, const char* uniform, float f)
{
    int location = glGetUniformLocation(shader, uniform);
    if (location == -1)
        {}//printf("Failed to get location of the uniform \"%s\"\n", uniform);
    glUniform1f(location, f);
}

unsigned int CreateShader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    printf("CreateShader called\n");
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    unsigned int program;

    std::ifstream file(vertexShaderPath, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file)
    {
	printf("CreateShader: Error opening source file %s\n", vertexShaderPath);
	return 0;
    }

    long filesize = file.tellg();
    file.seekg(0, std::ios_base::beg);
    vertexShaderSource.resize(filesize);

    file.read(vertexShaderSource.data(), filesize);

    file.close();

    file.open(fragmentShaderPath, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file)
    {
	printf("CreateShader: Error opening source file %s\n", vertexShaderPath);
	return 0;
    }

    filesize = file.tellg();
    file.seekg(0, std::ios_base::beg);
    fragmentShaderSource.resize(filesize);

    file.read(fragmentShaderSource.data(), filesize);

    file.close();


    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* src = vertexShaderSource.c_str();
    // printf("%s\n", vertexShaderSource.c_str());
    glShaderSource(vertexShader, 1, &src, NULL);
    src = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &src, NULL);

    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	printf("Vertex shader compilation failed!\nIn file %s:\n%s\n", vertexShaderPath, infoLog);
	throw std::exception();
	return 0;
    }

    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	printf("Fragment shader compilation failed!\nIn file %s:\n%s\n", fragmentShaderPath, infoLog);
	throw std::exception();
	return 0;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
	glGetProgramInfoLog(program, 512, NULL, infoLog);
	printf("Shader program linking failed!\n%s\n", infoLog);
	return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    printf("CreateShader finished\n");

    return program;
}
