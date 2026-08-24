#ifndef MESH_H
#define MESH_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <vector>

#include "engine3d/bufferlayout.hpp"

namespace e3d
{

class Mesh {
    public:
    Mesh(const void* vertexData, uint32_t vertexDataSize, const std::vector<unsigned int> &indices, const BufferLayout &layout);
    unsigned int GetID() const;
    void Bind() const;
    void Draw() const;

    private:
    unsigned int m_VBO, m_EBO, m_VAO;
    unsigned int m_indexCount;
};

} // End of namespace e3d

#endif
