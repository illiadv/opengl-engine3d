#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include <initializer_list>

class Texture
{
public:
    virtual ~Texture() = default;
    virtual void Bind(uint32_t slot = 0) = 0;
    virtual void Unbind(uint32_t slot = 0) = 0;
};

class Texture2D : public Texture
{
public:
    Texture2D(const char *path);
    Texture2D(void *data, unsigned int width, unsigned int height, unsigned int nChannels = 4);
    void Bind(uint32_t slot = 0) override;
    void Unbind(uint32_t slot = 0) override;
private:
    unsigned int m_ID;
};

class TextureArray2D : public Texture
{
public:
    TextureArray2D(const char *path);
    TextureArray2D(std::initializer_list<const char *> pathList, unsigned int nDesiredChannels = 0);
    void Bind(unsigned int slot = 0) override;
    void Unbind(unsigned int slot = 0) override;
private:
    unsigned int m_ID;
};

#endif
