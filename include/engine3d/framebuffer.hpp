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
    bool inheritScreenSize = true;
    bool enableDrawing = true;
    bool enableReading = true;
};

class Framebuffer
{
public:
    Framebuffer(FramebufferSpecification specification);
    bool IsComplete();
    void Resize(unsigned int width, unsigned int height);
    unsigned int GetID() const;
    const FramebufferSpecification &GetSpec() const;
    std::shared_ptr<Texture2D> GetTexture(int index) const;
private:

    struct Renderbuffer
    {
	unsigned int m_ID;
	TextureFormat format;
    };

    unsigned int m_ID;
    FramebufferSpecification m_spec;
    std::vector<std::shared_ptr<Texture2D>> m_textures;
    std::vector<Renderbuffer> m_renderbuffers;
};

} // End of namespace e3d

#endif
