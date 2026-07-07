#ifndef MODEL_H
#define MODEL_H

#include "engine3d/mesh.hpp"
#include "engine3d/material.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Model
{
public:
    std::vector<Mesh> meshes;
    std::vector<int> materialIndexes;
    std::vector<Material> materials;
    Model(std::string path, std::shared_ptr<Shader> shader, bool flipUVs = false);

private:
    std::string directory;
    std::shared_ptr<Shader> shader;
    void ProcessNode(aiNode *assimpNode, const aiScene *assimpScene);
    Mesh ProcessMesh(aiMesh *assimpMesh);
    void LoadMaterial(aiMaterial *assimpMaterial);
    std::shared_ptr<Texture2D> LoadMaterialTexture(aiString fllename);
    void LoadMaterialKeys(Material &material, aiMaterial *assimpMaterial);
};

#endif
