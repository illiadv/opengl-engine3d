#include "engine3d/framebuffer.hpp"
#include "util.hpp"

#include "glad/gl.h"
#include <cstdio>

Framebuffer::Framebuffer(int width, int height)
{
    glCall(glGenFramebuffers(1, &m_ID));
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));

    // Create texture color attachment
    m_texture = std::make_shared<Texture2D>(nullptr, width, height, 3);

    // Attach texture to framebuffer
    glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetID(), 0));

    // Create renderbuffer depth and stencil attachment
    unsigned int renderbuffer;
    glCall(glGenRenderbuffers(1, &renderbuffer));
    glCall(glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer));
    glCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600));
    glCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    // Attach renderbuffer
    glCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer));

    if (!IsComplete())
    {
	printf("Framebuffer is not complete!\n");
    }

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));  
}

bool Framebuffer::IsComplete()
{
    // need to bind first
    bool status = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    return status;
}

unsigned int Framebuffer::GetID() const
{
    return m_ID;
}

std::shared_ptr<Texture2D> Framebuffer::GetTexture() const
{
    return m_texture;
}
