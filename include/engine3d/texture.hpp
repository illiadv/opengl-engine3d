#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include <initializer_list>

namespace e3d
{

enum class TextureFormat
{
    RED,
    RGB8,
    RGBA8,
    RGB16F,

    DEPTH32F,
    DEPTH24_STENCIL8,
};

struct TextureSpecification
{
    TextureSpecification()
    {
    }
    TextureSpecification(TextureFormat format)
	: textureFormat(format)
    {
    }
    
    TextureFormat textureFormat;
};

int GetTextureFormat(TextureFormat format);

class Texture
{
public:
    virtual ~Texture() = default;
    virtual void Bind(uint32_t slot = 0) = 0;
};

class Texture2D : public Texture
{
public:
    Texture2D(const char *path);
    Texture2D(void *data, unsigned int width, unsigned int height, unsigned int nChannels = 4);
    Texture2D(void *data, unsigned int width, unsigned int height, TextureSpecification specification);
    void Bind(uint32_t slot = 0) override;
    void Resize(unsigned int width, unsigned int height);
    unsigned int GetID() const;
private:
    unsigned int m_ID;
    TextureSpecification m_spec;
};

class TextureArray2D : public Texture
{
public:
    TextureArray2D(const char *path);
    TextureArray2D(std::initializer_list<const char *> pathList, unsigned int nDesiredChannels = 0);
    void Bind(unsigned int slot = 0) override;
    unsigned int GetID() const;
private:
    unsigned int m_ID;
};

class TextureCubemap : public Texture
{
public:
    TextureCubemap(std::initializer_list<const char *> pathList, unsigned int nDesiredChannels = 0);
    void Bind(unsigned int slot = 0) override;
    unsigned int GetID() const;
private:
    unsigned int m_ID;
};

} // End of namespace e3d

#endif
