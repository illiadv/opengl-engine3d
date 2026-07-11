#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "glm/ext/vector_float3.hpp"

#include "engine3d/shader.hpp"
#include "engine3d/texture.hpp"


class Material
{
public:
    Material(std::shared_ptr<Shader> shader);

    void SetTexture(std::string name, std::shared_ptr<Texture> texture);
    void SetFloat(std::string name, float f);
    void SetVec3(std::string name, glm::vec3 v);

    void Bind() const;
    void Unbind() const;

    std::shared_ptr<Shader> GetShader() const;

private:
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, float> m_floats;
    std::unordered_map<std::string, glm::vec3> m_vec3s;

    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
};

#endif
