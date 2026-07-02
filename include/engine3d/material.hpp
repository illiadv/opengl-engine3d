#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "glm/ext/vector_float3.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "texture.hpp"


class Material
{
public:
    Material(std::shared_ptr<Shader> shader);

    void SetTexture(std::string name, Texture *texture, uint32_t slot);
    void uSetFloat(std::string name, float f);
    void uSetVec3(std::string name, glm::vec3 v);

    void Bind() const;

    std::shared_ptr<Shader> GetShader() const;

private:
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, float> m_floats;
    std::unordered_map<std::string, glm::vec3> m_vec3s;

    struct TextureData {
	Texture *texture;
	uint32_t slot;
    };

    std::unordered_map<std::string, TextureData> m_textures;
};

#endif
