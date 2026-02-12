#ifndef LIGHT_H
#define LIGHT_H

// #include <GL/glew.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"


class Light {
    public:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float type;

    float constant;
    float linear;
    float quadratic;

    Light(
	glm::vec3 direction,
	glm::vec3 ambient,
	glm::vec3 diffuse,
	glm::vec3 specular
    ) :
	position(direction),
	ambient(ambient),
	diffuse(diffuse),
	specular(specular)
    {
	type = 0.0f;
    }

    Light(
	glm::vec3 position,
	glm::vec3 ambient,
	glm::vec3 diffuse,
	glm::vec3 specular,
	float constant,
	float linear,
	float quadratic
    ) :
	position(position),
	ambient(ambient),
	diffuse(diffuse),
	specular(specular),
	constant(constant),
	linear(linear),
	quadratic(quadratic)
    {
	type = 1.0f;
    }

};

// class DirLight : public Light {
//
//     public:
//     glm::vec3 direction;
//
//     DirLight(
// 	glm::vec4 direction,
// 	glm::vec4 ambient,
// 	glm::vec4 diffuse,
// 	glm::vec4 specular
//     ) :
// 	Light(ambient, diffuse, specular),
// 	direction(direction)
//     {}
//
//     void UpdateUniforms(unsigned int shader)
//     {
// 	SetVec4(shader, "dirLight.diffuse", &diffuse[0]);
// 	SetVec4(shader, "dirLight.ambient", &ambient[0]);
// 	SetVec4(shader, "dirLight.specular", &specular[0]);
// 	SetVec4(shader, "dirLight.direction", &direction[0]);
//     }
// };

// class PointLight : public Light
// {
//     public:
//     glm::vec3 position;
//     float constant;
//     float linear;
//     float quadratic;
//
//     PointLight(
// 	glm::vec4 position,
// 	glm::vec4 ambient,
// 	glm::vec4 diffuse,
// 	glm::vec4 specular,
// 	float constant,
// 	float linear,
// 	float quadratic
//     ) :
// 	Light(ambient, diffuse, specular),
// 	position(position), constant(constant), linear(linear), quadratic(quadratic)
//     {}
//
//     void UpdateUniforms(unsigned int shader)
//     {
// 	SetVec3(shader, "pointLight.diffuse", &diffuse[0]);
// 	SetVec3(shader, "pointLight.ambient", &ambient[0]);
// 	SetVec3(shader, "pointLight.specular", &specular[0]);
// 	SetVec3(shader, "pointLight.position", &position[0]);
// 	SetFloat(shader, "pointLight.constant", constant);
// 	SetFloat(shader, "pointLight.linear", linear);
// 	SetFloat(shader, "pointLight.quadratic", quadratic);
//     }
// };

// class SpotLight : public PointLight
// {
//     public:
//     glm::vec3 direction;
//
//     float cutOff;
//     float outerCutOff;
//
//     SpotLight(
// 	glm::vec3 position,
// 	glm::vec3 direction,
// 	glm::vec3 ambient,
// 	glm::vec3 diffuse,
// 	glm::vec3 specular,
// 	float constant,
// 	float linear,
// 	float quadratic,
// 	float cutOff,
// 	float outerCutOff
//     ) :
// 	PointLight(position, ambient, diffuse, specular, constant, linear, quadratic),
// 	direction(direction),
// 	cutOff(cutOff), outerCutOff(outerCutOff)
//     {}
//
//     void UpdateUniforms(unsigned int shader)
//     {
// 	SetVec3(shader, "spotLight.direction", &direction[0]);
// 	SetVec3(shader, "spotLight.diffuse", &diffuse[0]);
// 	SetVec3(shader, "spotLight.ambient", &ambient[0]);
// 	SetVec3(shader, "spotLight.specular", &specular[0]);
// 	SetVec3(shader, "spotLight.position", &position[0]);
// 	SetFloat(shader, "spotLight.constant", constant);
// 	SetFloat(shader, "spotLight.linear", linear);
// 	SetFloat(shader, "spotLight.quadratic", quadratic);
// 	SetFloat(shader, "spotLight.cutOff", cutOff);
// 	SetFloat(shader, "spotLight.outerCutOff", outerCutOff);
//     }
// };
#endif
