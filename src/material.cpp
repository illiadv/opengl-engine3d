#include "engine3d/material.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shader.hpp"
#include "util.hpp"

Material::Material(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    m_shader = CreateShader(vertexShaderSource, fragmentShaderSource);

    unsigned int blockIndex = glGetUniformBlockIndex(m_shader, "Matricies");
    glCheckError();
    glUniformBlockBinding(m_shader, blockIndex, 0);
    glCheckError();

    blockIndex = glGetUniformBlockIndex(m_shader, "Lights");
    glCheckError();
    glUniformBlockBinding(m_shader, blockIndex, 1);
    glCheckError();

}
void Material::uSetFloat(std::string name, float f)
{
    m_floats[name] = f;
}

void Material::uSetVec3(std::string name, glm::vec3 v)
{
    m_vec3s[name] = v;
}

void Material::SetTexture(std::string name, Texture *texture, uint32_t slot)
{
    m_textures[name] = {texture, slot};
}

void Material::Bind() const
{
    for (auto& [name, value] : m_floats)
    {
	SetFloat(m_shader, name.c_str(), value);
    }

    for (auto& [name, value] : m_vec3s)
    {
	SetVec3(m_shader, name.c_str(), glm::value_ptr(value));
    }

    for (auto& [name, textureData] : m_textures)
    {
	textureData.texture->Bind(textureData.slot);
	SetInt(m_shader, name.c_str(), textureData.slot);
    }
}

const unsigned int& Material::GetShaderProgram() const
{
    return m_shader;
}
