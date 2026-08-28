#ifndef E3D_TRANSFORM_HPP
#define E3D_TRANSFORM_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/quaternion.hpp"

namespace e3d
{

class Transform
{
public:
    Transform &SetTranslation(const glm::vec3 &translation);
    Transform &Rotate(float radians, const glm::vec3 &axis);
    Transform &SetScale(glm::vec3 const &scale);
    glm::mat4 GetModelMatrix() const;
private:
    glm::vec3 m_translation = glm::vec3(0.0f);
    glm::quat m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f);
};

}

#endif
