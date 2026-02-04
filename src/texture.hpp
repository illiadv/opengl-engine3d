#ifndef TEXTURE_H
#define TEXTURE_H

// #include <GL/glew.h>
#include <glad/glad.h>
#include "stb_image.h"

unsigned int CreateTexture(const char* filename, int wrap = GL_REPEAT);

#endif
