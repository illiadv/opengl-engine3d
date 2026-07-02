#ifndef MODEL_H
#define MODEL_H

#include "mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelLoader
{
public:
    static std::vector<Mesh> Load(const char *path, bool flipUVs = false);
private:
    class Model
    {
    public:
	std::vector<Mesh> meshes;
	std::string directory;

	// std::vector<Texture> texturesLoaded;

	void LoadModel(std::string path, bool flipUVs);
	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	// std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	// Texture LoadMaterialTextures(aiString str, aiTextureType type);
    };
};

#endif
