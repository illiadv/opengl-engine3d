#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

#include "engine3d/shader.hpp"
#include "engine3d/texture.hpp"
#include "engine3d/renderstate.hpp"

namespace e3d
{

class Material
{
public:
    Material(std::shared_ptr<Shader> shader);

    void SetTexture(std::string name, std::shared_ptr<Texture> texture);
    void SetFloat(std::string name, float f);
    void SetVec3(std::string name, glm::vec3 v);
    void SetMat4(std::string name, glm::mat4 m);
    void SetRenderState(const RenderState &state);

    void Bind() const;

    std::shared_ptr<Shader> GetShader() const;
    unsigned int GetID() const;
    const RenderState &GetRenderState() const;

private:
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, float> m_floats;
    std::unordered_map<std::string, glm::vec3> m_vec3s;
    std::unordered_map<std::string, glm::mat4> m_mat4s;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    unsigned int m_id;
    RenderState m_renderState;
    

    std::shared_ptr<Texture2D> GetFallback(const std::string samplerName) const;

    static unsigned int s_id;
    static std::shared_ptr<Texture2D> s_whiteTexture;
    static std::shared_ptr<Texture2D> s_blackTexture;
    static std::shared_ptr<Texture2D> s_normalTexture;
};

} // End of namespace e3d

#endif
