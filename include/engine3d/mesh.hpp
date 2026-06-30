#ifndef MESH_H
#define MESH_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

#include <vector>

#include "glad/glad.h"
#include "engine3d/bufferlayout.hpp"

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
    // std::vector<Vertex> vertices;
    // std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Mesh(std::vector<Vertex> &verticies, std::vector<unsigned int> &indicies, std::vector<Texture> &texture);
    Mesh(const void* vertexData, uint32_t vertexDataSize, const std::vector<unsigned int> &indices, const BufferLayout &layout, std::vector<Texture> &texture);
    void Draw(unsigned int shader) const;

    private:
    unsigned int m_VBO, m_EBO, m_VAO;
    unsigned int m_indexCount;
};


#endif
