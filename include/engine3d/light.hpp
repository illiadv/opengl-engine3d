#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class DirectionalLight;
class PointLight;

class LightConsumer
{
public:
    virtual ~LightConsumer() = default;
    virtual void ConsumeDirectionalLight(const DirectionalLight* light) = 0;
    virtual void ConsumePointLight(const PointLight* light) = 0;
};

class Light
{
public:
    virtual ~Light() = default;
    virtual void Accept(LightConsumer& consumer) const = 0;
};

class DirectionalLight : public Light
{
public:
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    DirectionalLight(glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f),
		     glm::vec3 ambient = glm::vec3(0.2f),
		     glm::vec3 diffuse = glm::vec3(1.0f),
		     glm::vec3 specular = glm::vec3(1.0f))
	: direction(direction), ambient(ambient), diffuse(diffuse), specular(specular)
    {
    }
    
    void Accept(LightConsumer& consumer) const override
    {
	consumer.ConsumeDirectionalLight(this);
    }
};

class PointLight : public Light
{
public:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    PointLight(glm::vec3 position = glm::vec3(0.0f),
		     glm::vec3 ambient = glm::vec3(0.2f),
		     glm::vec3 diffuse = glm::vec3(1.0f),
		     glm::vec3 specular = glm::vec3(1.0f),
		     float range = 10.0f)
	: position(position), ambient(ambient), diffuse(diffuse), specular(specular)
    {
	SetRange(range);
    }

    void Accept(LightConsumer& consumer) const override
    {
	consumer.ConsumePointLight(this);
    }

    // Taken from https://wiki.ogre3d.org/Light+Attenuation+Shortcut
    void SetRange(float range)
    {
	constant = 1.0f;
	linear = 4.5 / range;
	quadratic = 75.0f / (range * range) ;
    }
};

#endif
