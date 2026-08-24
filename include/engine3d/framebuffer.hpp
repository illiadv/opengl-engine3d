#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <memory>
#include <vector>

#include "engine3d/texture.hpp"

namespace e3d {

enum class AttachmentType
{
    Texture,
    RenderBuffer
};

struct AttachmentSpecification
{
    TextureSpecification textureSpec;
    AttachmentType type;
};

struct FramebufferSpecification
{
    unsigned int width;
    unsigned int height;
    std::vector<AttachmentSpecification> attachments;
    bool enableDrawing = true;
    bool enableReading = true;
};

class Framebuffer
{
public:
    Framebuffer(FramebufferSpecification specification);
    bool IsComplete();
    unsigned int GetID() const;
    std::shared_ptr<Texture2D> GetTexture(int index) const;
private:
    unsigned int m_ID;
    std::vector<std::shared_ptr<Texture2D>> m_textures;
};

} // End of namespace e3d

#endif
