#ifndef E3D_LIGHT_HPP
#define E3D_LIGHT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine3d/transform.hpp"

namespace e3d
{

struct LightSpec
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class LightConsumer;

class Light
{
public:
    virtual ~Light() = default;
    virtual void Accept(LightConsumer& consumer) const = 0;
};

class DirectionalLight : public Light
{
public:
    Transform transform;
    LightSpec spec;

    DirectionalLight();
    
    void Accept(LightConsumer& consumer) const override;
};

class PointLight : public Light
{
public:
    Transform transform;
    LightSpec spec;

    float constant;
    float linear;
    float quadratic;

    PointLight();

    void Accept(LightConsumer& consumer) const override;

    void SetRange(float range);
};

class LightConsumer
{
public:
    virtual ~LightConsumer() = default;
    virtual void ConsumeDirectionalLight(const DirectionalLight* light) = 0;
    virtual void ConsumePointLight(const PointLight* light) = 0;
};

} // End of namespace e3d

#endif
