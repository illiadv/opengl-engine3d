#include "engine3d/framebuffer.hpp"
#include "util.hpp"

#include "glad/gl.h"
#include <cstdio>

namespace e3d
{

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
	    Renderbuffer renderbuffer;
	    renderbuffer.format = attachmentSpec.textureSpec.textureFormat;

	    glCall(glGenRenderbuffers(1, &renderbuffer.m_ID));
	    glCall(glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer.m_ID));
	    glCall(glRenderbufferStorage(GL_RENDERBUFFER, GetTextureFormat(renderbuffer.format), spec.width, spec.height));

	    glCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer.m_ID));


	    m_renderbuffers.push_back(renderbuffer);
	}
    }

    if (!spec.enableDrawing)
    {
	glDrawBuffer(GL_NONE);
    }
    if (!spec.enableReading)
    {
	glReadBuffer(GL_NONE);
    }

    if (!IsComplete())
    {
	printf("Framebuffer is not complete!\n");
    }

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));  
}

bool Framebuffer::IsComplete()
{
    // need to bind first
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) {
	case GL_FRAMEBUFFER_UNDEFINED:
	    printf("Error: framebuffer not complete: GL_FRAMEBUFFER_UNDEFINED\n");
	break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
	    printf("Error: framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
	break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
	    printf("Error: framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
	break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
	    printf("Error: framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n");
	break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
	    printf("Error: framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
	break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
	    printf("Error: framebuffer not complete: GL_FRAMEBUFFER_UNSUPPORTED\n");
	break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
	    printf("Error: framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n");
	break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
	    printf("Error: framebuffer not complete: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n");
	break;
    }
    bool complete = (status == GL_FRAMEBUFFER_COMPLETE);
    return complete;
}

void Framebuffer::Resize(unsigned int width, unsigned int height)
{
    for (auto texture : m_textures)
    {
	texture->Bind();
	texture->Resize(width, height);
    }

    for (auto renderbuffer: m_renderbuffers)
    {
	glCall(glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer.m_ID));
	glCall(glRenderbufferStorage(GL_RENDERBUFFER, GetTextureFormat(renderbuffer.format), width, height));
	glCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }

    m_spec.width = width;
    m_spec.height = height;
}

unsigned int Framebuffer::GetID() const
{
    return m_ID;
}

const FramebufferSpecification& Framebuffer::GetSpec() const
{
    return m_spec;
}

std::shared_ptr<Texture2D> Framebuffer::GetTexture(int index) const
{
    return m_textures[index];
}

}
