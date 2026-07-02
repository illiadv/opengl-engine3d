#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
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
    void Bind(uint32_t slot = 0) override;
    void Unbind(uint32_t slot = 0) override;
private:
    GLuint m_ID;
};

class TextureArray2D : public Texture
{
public:
    TextureArray2D(const char *path);
    TextureArray2D(std::initializer_list<const char *> pathList);
    void Bind(uint32_t slot = 0) override;
    void Unbind(uint32_t slot = 0) override;
private:
    GLuint m_ID;
};

#endif
