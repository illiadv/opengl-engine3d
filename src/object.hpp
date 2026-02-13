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
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;


    void SetNormalMat(glm::mat4 model, unsigned int shader) ;

    BaseModel *model;

    public:
    Material *material;

    // Object(Model* model);

    Object(BaseModel* model,
	Material* material = nullptr,
	glm::vec3 trans = glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3 scale = glm::vec3(1.0f),
	float rotDeg = 0.0f,
	glm::vec3 rotAxis = glm::vec3(1.0f)
	);

    BaseModel* GetModel();
    void SetMaterial(Material* material);
    void SetModel(glm::mat4 modelMat);
    glm::vec3 GetPosition();
    glm::vec3 GetScale();
    glm::quat GetRotation();
    void SetPosition(glm::vec3 trans);
    void SetRotation(glm::quat rotation);
    void Rotate(float deg, glm::vec3 axis);
    void SetScale(glm::vec3 scale);
    void Draw(unsigned int shader);
};
