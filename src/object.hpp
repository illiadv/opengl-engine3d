#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.hpp"
#include "util.hpp"

class Material
{
public:

    Material(unsigned int shader, float shininess)
    {
	this->shader = shader;
	this->shininess = shininess;

	unsigned int blockIndex = glGetUniformBlockIndex(shader, "Matricies");
	glUniformBlockBinding(shader, blockIndex, 0);

	blockIndex = glGetUniformBlockIndex(shader, "Lights");
	glCheckError();
	glUniformBlockBinding(shader, blockIndex, 1);
	glCheckError();
    }

    unsigned int shader = 0;
    float shininess = 0;
};

class Object
{
    private:
    // public:
    glm::vec3 position{};
    glm::quat rotation = glm::quat(glm::mat4(1.0f));
    glm::vec3 eulerRotation{};
    glm::vec3 scale{};


    void SetNormalMat(glm::mat4 model, unsigned int shader) ;
    void SetEulerRotationToModel();

    BaseModel *model;
    Material *material;

    public:

    Object(BaseModel* model,
	Material* material = nullptr,
	glm::vec3 trans = glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3 scale = glm::vec3(1.0f),
	float rotDeg = 0.0f,
	glm::vec3 rotAxis = glm::vec3(1.0f)
	);

    BaseModel* GetModel();
    glm::vec3 GetPosition();
    glm::vec3 GetScale();
    glm::quat GetRotation();
    glm::vec3 GetEulerRotation();
    Material* GetMaterial(void);
    void SetPosition(glm::vec3 trans);
    void SetRotation(glm::quat rotation);
    void SetEulerRotation(glm::vec3 );
    void Rotate(float deg, glm::vec3 axis);
    void SetScale(glm::vec3 scale);
    void SetMaterial(Material* material);
    void SetModel(glm::mat4 modelMat);
    void Draw(unsigned int shader);
};

#endif
