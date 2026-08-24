#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>

namespace e3d
{

class Shader
{
public:
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
    void SetMat4(const char* uniform, const float* matrix);
    void SetMat3(const char* uniform, const float* matrix);
    void SetVec3(const char* uniform, const float* vec3);
    void SetVec3(const char* uniform, float x, float y, float z);
    void SetVec4(const char* uniform, const float* vec4);
    void SetFloat(const char* uniform, float f);
    void SetInt(const char* uniform, int i);
    void Bind();
    void Reflect();
    unsigned int GetID() const;
    const std::vector<std::string> GetSamplers() const;
private:
    unsigned int m_ID;
    std::vector<std::string> m_samplers;
};

} // End of namespace e3d

#endif
