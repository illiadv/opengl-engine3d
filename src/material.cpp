#include "engine3d/material.hpp"
#include "shader.hpp"
#include "util.hpp"

Material::Material(const char *vertexShaderSource, const char *fragmentShaderSource, float shininess)
{
    shader = CreateShader(vertexShaderSource, fragmentShaderSource);
    this->shininess = shininess;

    unsigned int blockIndex = glGetUniformBlockIndex(shader, "Matricies");
    glCheckError();
    glUniformBlockBinding(shader, blockIndex, 0);
    glCheckError();

    blockIndex = glGetUniformBlockIndex(shader, "Lights");
    glCheckError();
    glUniformBlockBinding(shader, blockIndex, 1);
    glCheckError();

}

const unsigned int& Material::GetShaderProgram() const
{
    return shader;
}
