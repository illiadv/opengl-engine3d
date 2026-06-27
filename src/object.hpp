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

#endif
