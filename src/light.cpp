#include "engine3d/light.hpp"

namespace e3d
{

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::DirectionalLight(Transform transform, LightSpec spec)
    : transform(transform), spec(spec)
{
}

PointLight::PointLight()
{
}

PointLight::PointLight(Transform transform, LightSpec spec, float range)
    : transform(transform), spec(spec)
{
    SetRange(range);
}

void DirectionalLight::Accept(LightConsumer& consumer) const
{
    consumer.ConsumeDirectionalLight(this);
}

void PointLight::Accept(LightConsumer& consumer) const
{
    consumer.ConsumePointLight(this);
}

// Taken from https://wiki.ogre3d.org/Light+Attenuation+Shortcut
void PointLight::SetRange(float range)
{
    constant = 1.0f;
    linear = 4.5 / range;
    quadratic = 75.0f / (range * range) ;
}

}
