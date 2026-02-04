#ifndef MESH_H
#define MESH_H

// #include <GL/glew.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

enum TextureType {
    diffuse, specular, emission
};

struct Texture {
    unsigned int id;
    TextureType type;
    std::string path;
};

class Mesh {
    public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> &verticies, std::vector<unsigned int> &indicies, std::vector<Texture> &texture);
    void Draw(unsigned int shader);

    private:
    unsigned int VBO, EBO, VAO;
    void SetupMesh();
};


#endif
