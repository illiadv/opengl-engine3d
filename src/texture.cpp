#include <stdexcept>
#include <cstring>

#include "glad/gl.h"
#include "engine3d/texture.hpp"
#include "util.hpp"
#include "stb_image.h"

unsigned char *LoadTextureFile(const char *path, int &width, int &height, int &nChannels, int nDesiredChannels = 0)
{
    unsigned char *textureData = stbi_load(path, &width, &height, &nChannels, nDesiredChannels);

    if (!textureData)
    {
	throw (std::runtime_error(std::string("Failed to load texture") + path + "\n"));
    }

    if (nDesiredChannels > 0)
    {
	nChannels = nDesiredChannels;
    }

    printf("Loaded texture %s %dx%d, %d channels\n", path, width, height, nChannels);
    return textureData;
}

GLint GetSourceFormat(int nChannels)
{
    switch (nChannels) {
	case 1:
	    return GL_RED;
	case 3:
	    return GL_RGB;
	default:
	    return GL_RGBA;
    }
}

Texture2D::Texture2D(const char *path)
{
    int width, height, nChannels;
    unsigned char *textureData = LoadTextureFile(path, width, height, nChannels);

    unsigned int sourceFormat = GetSourceFormat(nChannels);

    glGenTextures(1, &m_ID);

    glBindTexture(GL_TEXTURE_2D, m_ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, sourceFormat, width, height, 0, sourceFormat, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(textureData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(void *data, unsigned int width, unsigned int height, unsigned int nChannels)
{
    unsigned int sourceFormat = GetSourceFormat(nChannels);

    glGenTextures(1, &m_ID);

    glBindTexture(GL_TEXTURE_2D, m_ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    glTexImage2D(GL_TEXTURE_2D, 0, sourceFormat, width, height, 0, sourceFormat, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind(uint32_t slot)
{
    glCall(glActiveTexture(GL_TEXTURE0 + slot));
    // glCall(glUniform1i(glGetUniformLocation(shader, name.c_str()), i));
    glCall(glBindTexture(GL_TEXTURE_2D, m_ID));
}

unsigned int Texture2D::GetID() const
{
    return m_ID;
}

TextureArray2D::TextureArray2D(std::initializer_list<const char*> filenames, unsigned int nDesiredChannels)
{
    unsigned char *texArray = nullptr;
    int width, height, nChannels;

    int i = 0;
    for (auto filename : filenames)
    {
	unsigned char *textureData = LoadTextureFile(filename, width, height, nChannels, nDesiredChannels);

	if (texArray == nullptr)
	{
	    texArray = new unsigned char[width * height * nChannels * filenames.size()];
	}
	
	memcpy(texArray + (width * height * nChannels) * i, textureData, width * height * nChannels);
	stbi_image_free(textureData);
	i++;
    }

    unsigned int sourceFormat = GetSourceFormat(nChannels);

    glCall(glGenTextures(1, &m_ID));


    glCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID));

    
    glCall(glTexImage3D(GL_TEXTURE_2D_ARRAY,
		 0,                 // mipmap level
		 GL_RGBA8,          // gpu texel format
		 width,             // width
		 height,            // height
		 filenames.size(),  // depth
		 0,                 // border
		 sourceFormat,	    // cpu pixel format
		 GL_UNSIGNED_BYTE,  // cpu pixel coord type
		 texArray));        // pixel data
    glCall(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));

    if (texArray != nullptr)
	delete[] texArray;
}

void TextureArray2D::Bind(uint32_t slot)
{
    glCall(glActiveTexture(GL_TEXTURE0 + slot));
    // glCall(glUniform1i(glGetUniformLocation(shader, name.c_str()), i));
    glCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID));
}

unsigned int TextureArray2D::GetID() const
{
    return m_ID;
}

