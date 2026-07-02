#include <stdexcept>
#include <cstring>

#include "engine3d/texture.hpp"
#include "util.hpp"
#include "stb_image.h"

unsigned char *LoadTextureFile(const char *path, int &width, int &height, int &nChannels)
{
    unsigned char *textureData = stbi_load(path, &width, &height, &nChannels, 0);

    if (!textureData)
    {
	throw (std::runtime_error(std::string("Failed to load texture") + path + "\n"));
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

void Texture2D::Bind(uint32_t slot)
{
    glCall(glActiveTexture(GL_TEXTURE0 + slot));
    // glCall(glUniform1i(glGetUniformLocation(shader, name.c_str()), i));
    glCall(glBindTexture(GL_TEXTURE_2D, m_ID));
}

void Texture2D::Unbind(uint32_t slot)
{
    glCall(glActiveTexture(GL_TEXTURE0 + slot));
    glCall(glBindTexture(GL_TEXTURE_2D, 0));
}

unsigned int CreateTextureArray(std::initializer_list<const char*> filenames)
{
    unsigned char *texArray = nullptr;
    int width, height, nChannels;

    int i = 0;
    for (auto filename : filenames)
    {
	// NOTE: for some files it may be necessary to force certain nChannels (e.g. 4)
	unsigned char *textureData = LoadTextureFile(filename, width, height, nChannels);

	if (texArray == nullptr)
	{
	    texArray = new unsigned char[width * height * nChannels * filenames.size()];
	}
	
	memcpy(texArray + (width * height * nChannels) * i, textureData, width * height * nChannels);
	stbi_image_free(textureData);
	i++;
    }


    unsigned int sourceFormat = 0;
    switch (nChannels) {
	case 1:
	    sourceFormat = GL_RED;
	    break;
	case 3:
	    sourceFormat = GL_RGB;
	    break;
	case 4:
	    sourceFormat = GL_RGBA;
	    break;
	default:
	    printf("Unknown image format with %d channels", nChannels);
	    break;
    }

    unsigned int texture;
    glCall(glGenTextures(1, &texture));


    glCall(glBindTexture(GL_TEXTURE_2D_ARRAY, texture));

    
    glCall(glTexImage3D(GL_TEXTURE_2D_ARRAY,
		 0,                 // mipmap level
		 GL_RGBA8,          // gpu texel format
		 width,             // width
		 height,            // height
		 filenames.size(),  // depth
		 0,                 // border
		 GL_RGBA,           // cpu pixel format
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

    return texture;
}

void TextureArray2D::Bind(uint32_t slot)
{
    glCall(glActiveTexture(GL_TEXTURE0 + slot));
    // glCall(glUniform1i(glGetUniformLocation(shader, name.c_str()), i));
    glCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID));
}

void TextureArray2D::Unbind(uint32_t slot)
{
    glCall(glActiveTexture(GL_TEXTURE0 + slot));
    glCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
}
