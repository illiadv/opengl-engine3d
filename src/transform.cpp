#include "engine3d/transform.hpp"

namespace e3d
{

Transform &Transform::SetTranslation(const glm::vec3 &translation)
{
    m_translation = translation;
    m_dirty = true;
    return *this;
};

Transform &Transform::SetRotation(const glm::vec3 &eulerAngles)
{
    m_rotation = glm::quat(eulerAngles);
    m_dirty = true;
    return *this;
};

Transform &Transform::SetScale(float scale)
{
    m_scale = glm::vec3(scale);
    m_dirty = true;
    return *this;
};

Transform &Transform::SetScale(const glm::vec3 &scale)
{
    m_scale = scale;
    m_dirty = true;
    return *this;
};

Transform &Transform::Rotate(float angle, const glm::vec3 &axis)
{
    glm::vec3 axisNorm = glm::normalize(axis);
    m_rotation = glm::rotate(m_rotation, angle, axisNorm);
    m_dirty = true;
    return *this;
};


glm::mat4 Transform::GetModelMatrix() const
{
    if (m_dirty) {
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, m_translation);
	transform = transform * glm::toMat4(m_rotation);
	transform = glm::scale(transform, m_scale);
	m_modelMatrix = transform;
	m_dirty = false;
    }
    return m_modelMatrix;
}

}
