#include "engine3d/framebuffer.hpp"
#include "util.hpp"

#include "glad/gl.h"
#include <cstdio>


Framebuffer::Framebuffer(FramebufferSpecification spec)
{
    glCall(glGenFramebuffers(1, &m_ID));
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));

    for (unsigned int index = 0; index < spec.attachments.size(); index++)
    {
	const AttachmentSpecification &attachmentSpec = spec.attachments[index];

	GLenum attachment;
	switch (attachmentSpec.textureSpec.textureFormat) {
	    case TextureFormat::DEPTH24_STENCIL8:
		attachment = GL_DEPTH_STENCIL_ATTACHMENT;
	    break;
	    case TextureFormat::DEPTH32F:
		attachment = GL_DEPTH_ATTACHMENT;
	    break;
	    default:
		attachment = GL_COLOR_ATTACHMENT0 + index;
	    break;
	}

	if (attachmentSpec.type == AttachmentType::Texture)
	{
	    auto texture = std::make_shared<Texture2D>(nullptr, spec.width, spec.height, attachmentSpec.textureSpec.textureFormat);

	    glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->GetID(), 0));
	    m_textures.push_back(texture);
	}
	else
	{
	    unsigned int renderbuffer;
	    glCall(glGenRenderbuffers(1, &renderbuffer));
	    glCall(glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer));
	    glCall(glRenderbufferStorage(GL_RENDERBUFFER, GetTextureSourceFormat(attachmentSpec.textureSpec.textureFormat), spec.width, spec.height));

	    glCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer));
	}

	if (!spec.enableDrawing)
	{
	    glDrawBuffer(GL_NONE);
	}
	if (!spec.enableReading)
	{
	    glReadBuffer(GL_NONE);
	}

	glCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }


    // Attach renderbuffer

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

std::shared_ptr<Texture2D> Framebuffer::GetTexture(int index) const
{
    return m_textures[index];
}
