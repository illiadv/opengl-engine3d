#include "engine3d/material.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shader.hpp"
#include "util.hpp"

Material::Material(std::shared_ptr<Shader> shader)
    : m_shader(shader)
{
    unsigned int blockIndex = glGetUniformBlockIndex(m_shader->GetID(), "Matricies");
    glCheckError();
    glUniformBlockBinding(m_shader->GetID(), blockIndex, 0);
    glCheckError();

    blockIndex = glGetUniformBlockIndex(m_shader->GetID(), "Lights");
    glCheckError();
    glUniformBlockBinding(m_shader->GetID(), blockIndex, 1);
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
    m_shader->Bind();

    for (auto& [name, value] : m_floats)
    {
	m_shader->SetFloat(name.c_str(), value);
    }

    for (auto& [name, value] : m_vec3s)
    {
	m_shader->SetVec3(name.c_str(), glm::value_ptr(value));
    }

    for (auto& [name, textureData] : m_textures)
    {
	textureData.texture->Bind(textureData.slot);
	m_shader->SetInt(name.c_str(), textureData.slot);
    }
}

std::shared_ptr<Shader> Material::GetShader() const
{
    return m_shader;
}
