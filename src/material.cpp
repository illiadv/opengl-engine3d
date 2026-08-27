#include "glad/gl.h"
#include "engine3d/material.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "engine3d/shader.hpp"
#include "util.hpp"

namespace e3d
{

std::shared_ptr<Texture2D> Material::s_whiteTexture;
std::shared_ptr<Texture2D> Material::s_blackTexture;
std::shared_ptr<Texture2D> Material::s_normalTexture;

unsigned int Material::s_id = 0;

Material::Material(std::shared_ptr<Shader> shader)
    : m_shader(shader)
{
    s_id++;
    m_id = s_id;

    if (s_whiteTexture == nullptr)
    {
	uint32_t whitePixel = 0xFFFFFFFF;
	s_whiteTexture = std::make_shared<Texture2D>(&whitePixel, 1, 1);
    }

    if (s_blackTexture == nullptr)
    {
	uint32_t blackPixel = 0x00000000;
	s_blackTexture = std::make_shared<Texture2D>(&blackPixel, 1, 1);
    }

    if (s_blackTexture == nullptr)
    {
	uint32_t normalPixel = 0x8080FFFF;
	s_normalTexture = std::make_shared<Texture2D>(&normalPixel, 1, 1);
    }

}
void Material::SetFloat(std::string name, float f)
{
    m_floats[name] = f;
}

void Material::SetVec3(std::string name, glm::vec3 v)
{
    m_vec3s[name] = v;
}

void Material::SetMat4(std::string name, glm::mat4 m)
{
    m_mat4s[name] = m;
}

void Material::SetTexture(std::string name, std::shared_ptr<Texture> texture)
{
    m_textures[name] = texture;
}

void Material::Bind() const
{
    for (auto& [name, value] : m_floats)
    {
	m_shader->SetFloat(name.c_str(), value);
    }

    for (auto& [name, value] : m_vec3s)
    {
	m_shader->SetVec3(name.c_str(), glm::value_ptr(value));
    }

    for (auto& [name, value] : m_mat4s)
    {
	m_shader->SetMat4(name.c_str(), glm::value_ptr(value));
    }

    // Look through all the samplers the shader has and bind a texture to them
    int slot = 0;
    for (auto sampler : m_shader->GetSamplers())
    {
	// Find a texture with a corresponding name
	auto it = m_textures.find(sampler);

	// If a texture is found, bind it to current slot
	if (it != m_textures.end())
	{
	    it->second->Bind(slot);
	}
	else
	{
	    auto fallback = GetFallback(sampler);
	    fallback->Bind(slot);
	}
	m_shader->SetInt(sampler.c_str(), slot);
	slot++;
    }

}

std::shared_ptr<Texture2D> Material::GetFallback(const std::string samplerName) const
{
    if (samplerName.find("normal") != std::string::npos)
    {
	return s_normalTexture;
    }

    if (samplerName.find("emission") != std::string::npos ||
	samplerName.find("displacement") != std::string::npos)
    {
	return s_blackTexture;
    }

    return s_whiteTexture;
}

std::shared_ptr<Shader> Material::GetShader() const
{
    return m_shader;
}

unsigned int Material::GetID() const
{
    return m_id;
}

void Material::SetRenderState(const RenderState &state)
{
    m_renderState = state;
}

const RenderState &Material::GetRenderState() const
{
    return m_renderState;
}

}
