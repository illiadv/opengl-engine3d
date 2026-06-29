#include "engine3d/mesh.hpp"
#include "util.hpp"

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &texture)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = texture;

    SetupMesh();
}

void Mesh::SetupMesh()
{
    // OpenGL objects
    glCall(glGenBuffers(1, &VBO));
    glCall(glGenVertexArrays(1, &VAO));
    glCall(glGenBuffers(1, &EBO));

    // bind VAO
    glCall(glBindVertexArray(VAO));
    // bind VBO as ARRAY_BUFFER
    glCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    // set VBO data
    glCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));

    // bind EBO as ELEMENT_ARRAY_BUFFER
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    // set EBO data
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));

    // set vertex attrib pointers
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
    glCall(glEnableVertexAttribArray(0));
    glCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, normal)));
    glCall(glEnableVertexAttribArray(1));
    glCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, texCoord)));
    glCall(glEnableVertexAttribArray(2));
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
    glCall(glBindVertexArray(VAO));
    // Draw
    glCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0));

    // unbind arrays
    glCall(glBindVertexArray(0));

    // Unbind textures from slots
    for (size_t i = 0; i < textures.size(); i++) {
	glCall(glActiveTexture(GL_TEXTURE0 + i));
	glCall(glBindTexture(GL_TEXTURE_2D, 0));
    }
}
