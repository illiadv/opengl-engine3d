#ifndef UTIL_HPP
#define UTIL_HPP

namespace e3d
{

unsigned int glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#define glCall(x) while(glGetError() != GL_NO_ERROR); x; glCheckError()

}

#endif
