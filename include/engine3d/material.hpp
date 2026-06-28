#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Material
{
public:
    Material(const char *vertexShaderSource, const char *fragmentShaderSource, float shininess);
    const unsigned int& GetShaderProgram() const;

    float shininess = 0;
private:
    unsigned int shader = 0;
};

#endif
