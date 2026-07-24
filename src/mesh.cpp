#include <stdexcept>

#include "glad/gl.h"
#include "engine3d/mesh.hpp"
#include "util.hpp"

Mesh::Mesh(const void* vertexData, uint32_t vertexDataSize, const std::vector<unsigned int> &indices, const BufferLayout &layout)
{
    if (!vertexData && vertexDataSize > 0) {
	throw std::invalid_argument("Cannot construct mesh with NULL vertexData and non-zero size");
    }

    // Create OpenGL objects
    glCall(glGenBuffers(1, &m_VBO));
    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glGenBuffers(1, &m_EBO));

    // Bind VAO first
    glCall(glBindVertexArray(m_VAO));
    // Bind VBO as ARRAY_BUFFER
    glCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    // Set VBO data
    glCall(glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW));

    // Sind EBO as ELEMENT_ARRAY_BUFFER
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
    // Set EBO data
    if (indices.size())
    {
	glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
    }
    m_indexCount = indices.size();

    // Set vertex attrib pointers based on the layout description
    unsigned int elementIndex = 0;
    for (auto element : layout.GetElements())
    {
	if (element.type == AttributeType::Float)
	{
	    glCall(glVertexAttribPointer(elementIndex,
		    element.count,
		    element.GetGLBaseType(),
		    element.normalized ? GL_TRUE : GL_FALSE,
		    layout.GetStride(),
		    (void*)(uintptr_t)element.offset));
	}
	else
	{
	    glCall(glVertexAttribIPointer(elementIndex,
		    element.count,
		    element.GetGLBaseType(),
		    layout.GetStride(),
		    (void*)(uintptr_t)element.offset));
	}


	glCall(glEnableVertexAttribArray(elementIndex));
	elementIndex++;
    }
}

void Mesh::Draw() const
{
    glCall(glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0));
}

void Mesh::Bind() const
{
    glCall(glBindVertexArray(m_VAO));
}

unsigned int Mesh::GetID() const
{
    return m_VAO;
}
