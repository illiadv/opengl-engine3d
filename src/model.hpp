#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include "assimp/material.h"
#include "texture.hpp"
#include "mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
    public:
	Model(const char *path, bool flipUVs = false)
	{
	    printf("Constructor for Model %s\n", path);
	    LoadModel(path, flipUVs);
	}
	void Draw(unsigned int shader);
	const char* GetDirectory();

    private:
	std::vector<Mesh> meshes;
	std::string directory;

	std::vector<Texture> texturesLoaded;

	void LoadModel(std::string path, bool flipUVs);
	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	// std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	Texture LoadMaterialTextures(aiString str, aiTextureType type);
};


#endif
