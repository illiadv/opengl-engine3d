#include "object.hpp"
#include "shader.hpp"
#include <glm/gtx/euler_angles.hpp>

// Object::Object(Model* model)
// {
//     this->model = model;
// }

Object::Object(BaseModel* model,
	Material* material,
	glm::vec3 trans,
	glm::vec3 scale,
	float rotDeg,
	glm::vec3 rotAxis
	)
{
    this->model = model;
    this->material = material;
    SetPosition(trans);
    Rotate(rotDeg, rotAxis);
    SetScale(scale);
    SetEulerRotationToModel();
}

void Object::SetNormalMat(glm::mat4 modelMat, unsigned int shader)
{
    glm::mat3 normalMat;
    normalMat = glm::transpose(glm::inverse(/* view * */ modelMat));
    SetMat3(shader, "normalMat", glm::value_ptr(normalMat));
}

BaseModel* Object::GetModel()
{
    return model;
}

glm::vec3 Object::GetPosition()
{
    return position;
}

glm::quat Object::GetRotation()
{
    return rotation;
}

glm::vec3 Object::GetScale()
{
    return scale;
}

void Object::SetPosition(glm::vec3 trans)
{
    position = trans;
}

void Object::SetEulerRotationToModel()
{
    glm::mat4 m = glm::mat4_cast(rotation);
    float y, p, r;
    glm::extractEulerAngleXYZ(m, y, p, r);
    eulerRotation = glm::vec3(glm::degrees(y), glm::degrees(p), glm::degrees(r));
}

void Object::Rotate(float deg, glm::vec3 axis)
{
    rotation = glm::rotate(rotation, deg, axis);
    SetEulerRotationToModel();
}

void Object::SetScale(glm::vec3 scale)
{
    this->scale = scale;
}

void Object::SetMaterial(Material* material)
{
    this->material = material;
}

void Object::SetRotation(glm::quat rotation)
{
    this->rotation = rotation;
    // SetEulerRotationToModel();
}

void Object::Draw(unsigned int shader)
{
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, position);
    modelMat = modelMat * glm::mat4_cast(rotation);
    modelMat = glm::scale(modelMat, scale);

    SetMat4(shader, "model", glm::value_ptr(modelMat));
    SetNormalMat(modelMat, shader);

    model->Draw(shader);
    // glUseProgram(0);
}
