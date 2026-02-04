#include "texture.hpp"

unsigned int CreateTexture(const char* filename, int wrap)
{
    int width, height, nChannels;
    unsigned char *textureData = stbi_load(filename, &width, &height, &nChannels, 0);

    if (!textureData)
    {
	printf("Failed to load texture %s\n", filename);
	return 0;
    }
    else
    {
	printf("Loaded texture %s %dx%d, %d channels\n", filename, width, height, nChannels);
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
    glGenTextures(1, &texture);


    glBindTexture(GL_TEXTURE_2D, texture);

    // how exactly does this work?
    // afair it works for the bound one only
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, /*GL_RGB*/ sourceFormat, width, height, 0, sourceFormat, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(textureData);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
