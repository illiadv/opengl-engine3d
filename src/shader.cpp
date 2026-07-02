#include "engine3d/shader.hpp"
#include <stdio.h>
#include <fstream>
#include <string>


void Shader::SetMat4(const char* uniform, float* matrix)
{
    int location = glGetUniformLocation(m_ID, uniform);
    if (location == -1)
    {
        throw std::runtime_error(std::string("Failed to get location of the uniform ") + uniform + "\n");
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

void Shader::SetMat3(const char* uniform, float* matrix)
{
    int location = glGetUniformLocation(m_ID, uniform);
    if (location == -1)
    {
        throw std::runtime_error(std::string("Failed to get location of the uniform ") + uniform + "\n");
    }
    glUniformMatrix3fv(location, 1, GL_FALSE, matrix);
}

void Shader::SetVec3(const char* uniform, const float* vec3)
{
    int location = glGetUniformLocation(m_ID, uniform);
    if (location == -1)
    {
        throw std::runtime_error(std::string("Failed to get location of the uniform ") + uniform + "\n");
    }
    glUniform3fv(location, 1, vec3);
}

void Shader::SetVec3(const char* uniform, float x, float y, float z)
{
    int location = glGetUniformLocation(m_ID, uniform);
    if (location == -1)
    {
        throw std::runtime_error(std::string("Failed to get location of the uniform ") + uniform + "\n");
    }
    glUniform3f(location, x, y, z);
}

void Shader::SetVec4(const char* uniform, const float* vec4)
{
    int location = glGetUniformLocation(m_ID, uniform);
    if (location == -1)
    {
        throw std::runtime_error(std::string("Failed to get location of the uniform ") + uniform + "\n");
    }
    glUniform4fv(location, 1, vec4);
}

void Shader::SetFloat(const char* uniform, float f)
{
    int location = glGetUniformLocation(m_ID, uniform);
    if (location == -1)
    {
        throw std::runtime_error(std::string("Failed to get location of the uniform ") + uniform + "\n");
    }
    glUniform1f(location, f);
}

void Shader::SetInt(const char* uniform, int i)
{
    int location = glGetUniformLocation(m_ID, uniform);
    if (location == -1)
    {
        throw std::runtime_error(std::string("Failed to get location of the uniform ") + uniform + "\n");
    }
    glUniform1i(location, i);
}

void ReadSourceFile(const char* path, std::string &dest)
{
    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file)
    {
	throw std::runtime_error(std::string("CreateShader: Error opening source file ") + path + "\n");
    }

    long filesize = file.tellg();
    file.seekg(0, std::ios_base::beg);
    dest.resize(filesize);

    file.read(dest.data(), filesize);

    file.close();
}

void Shader::Bind()
{
    glUseProgram(m_ID);
}

GLuint Shader::GetID() const
{
    return m_ID;
}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    ReadSourceFile(vertexShaderPath, vertexShaderSource);
    ReadSourceFile(fragmentShaderPath, fragmentShaderSource);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* src = vertexShaderSource.c_str();
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
	printf("Error in file %s:\n%s\n", vertexShaderPath, infoLog);
	throw std::runtime_error("Vertex shader compilation failed!\n");
    }

    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	printf("Error in file %s:\n%s\n", fragmentShaderPath, infoLog);
	throw std::runtime_error("Fragment shader compilation failed!\n");
    }

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertexShader);
    glAttachShader(m_ID, fragmentShader);
    glLinkProgram(m_ID);

    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if(!success) {
	glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
	printf("Error: %s\n", infoLog);
	throw std::runtime_error("Shader program linking failed!\n");
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
