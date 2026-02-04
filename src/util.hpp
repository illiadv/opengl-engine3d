#ifndef UTIL_HPP
#define UTIL_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stdio.h>

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
	bool unknown = 0;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            // case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            // case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }

	if (unknown)
	    printf("GL Error: %x (%s, %d\n", errorCode, file, line);
	else
	    printf("GL Error: %s (%s, %d\n", error.c_str(), file, line);
    }
    return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#define glCall(x) x; glCheckError()

#endif
