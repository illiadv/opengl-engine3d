#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <memory>

#include "engine3d/texture.hpp"

class Framebuffer
{
public:
    Framebuffer(int width, int height);
    bool IsComplete();
    unsigned int GetID() const;
    std::shared_ptr<Texture2D> GetTexture() const;
private:
    unsigned int m_ID;
    std::shared_ptr<Texture2D> m_texture;
};

#endif
