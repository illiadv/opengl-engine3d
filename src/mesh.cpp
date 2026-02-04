#include "mesh.hpp"

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
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    // bind VAO
    glBindVertexArray(VAO);
    // bind VBO as ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set VBO data
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // bind EBO as ELEMENT_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // set EBO data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set vertex attrib pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
}

void Mesh::Draw(unsigned int shader)
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
	glActiveTexture(GL_TEXTURE0 + i);
	// we only actually have to do this once?
	glUniform1i(glGetUniformLocation(shader, name.c_str()), i);
	glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // bind arrays
    glBindVertexArray(VAO);
    // Draw
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // unbind arrays
    glBindVertexArray(0);

    // Unbind textures from slots
    for (size_t i = 0; i < textures.size(); i++) {
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, 0);
    }
}
