#include "engine3d/transform.hpp"

namespace e3d
{

e3d::Transform& Transform::SetTranslation(const glm::vec3& translation)
{
    m_translation = translation;
    return *this;
};

e3d::Transform& Transform::Rotate(float angle, const glm::vec3 &axis)
{
    glm::vec3 axisNorm = glm::normalize(axis);
    m_rotation = glm::rotate(m_rotation, angle, axisNorm);
    return *this;
};

e3d::Transform& Transform::SetScale(const glm::vec3 &scale)
{
    m_translation = scale;
    return *this;
};

glm::mat4 Transform::GetModelMatrix() const
{
    glm::mat4 matrix(1.0f);
    matrix = glm::translate(matrix, m_translation);
    matrix = matrix * glm::toMat4(m_rotation);
    matrix = glm::scale(matrix, m_scale);
    return matrix;
}

}
