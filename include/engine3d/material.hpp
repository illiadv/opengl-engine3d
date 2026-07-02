#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <unordered_map>

#include "glm/ext/vector_float3.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "texture.hpp"


class Material
{
public:
    Material(const char *vertexShaderSource, const char *fragmentShaderSource);
    Material(unsigned int shaderProgram);

    void SetTexture(std::string name, Texture *texture, uint32_t slot);
    void uSetFloat(std::string name, float f);
    void uSetVec3(std::string name, glm::vec3 v);

    void Bind() const;

    const unsigned int& GetShaderProgram() const;

private:
    unsigned int m_shader = 0;
    std::unordered_map<std::string, float> m_floats;
    std::unordered_map<std::string, glm::vec3> m_vec3s;

    struct TextureData {
	Texture *texture;
	uint32_t slot;
    };

    std::unordered_map<std::string, TextureData> m_textures;
};

#endif
