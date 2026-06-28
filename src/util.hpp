#ifndef UTIL_HPP
#define UTIL_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#define glCall(x) while(glGetError() != GL_NO_ERROR); x; glCheckError()

#endif
