#include "engine3d/mesh.hpp"
#include "util.hpp"

Mesh::Mesh(const void* vertexData, uint32_t vertexDataSize, const std::vector<unsigned int> &indices, const BufferLayout &layout, std::vector<Texture> &texture)
{
    this->textures = texture;

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
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
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

void Mesh::Draw(unsigned int shader) const
{
    // bind textures to slots
    for (size_t i = 0; i < textures.size(); i++) {
	std::string name = "material.";

	switch (textures[i].type) {
	    case TextureType::diffuse:
		name += "diffuse"; break;
	    case TextureType::specular:
		name += "specular"; break;
	    case TextureType::emission:
		name += "emission"; break;
	}
	glCall(glActiveTexture(GL_TEXTURE0 + i));
	// we only actually have to do this once?
	glCall(glUniform1i(glGetUniformLocation(shader, name.c_str()), i));
	glCall(glBindTexture(GL_TEXTURE_2D, textures[i].id));
    }

    // bind arrays
    glCall(glBindVertexArray(m_VAO));
    // Draw
    glCall(glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0));

    // unbind arrays
    glCall(glBindVertexArray(0));

    // Unbind textures from slots
    for (size_t i = 0; i < textures.size(); i++) {
	glCall(glActiveTexture(GL_TEXTURE0 + i));
	glCall(glBindTexture(GL_TEXTURE_2D, 0));
    }
}
