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
    Material material;
    Model(std::string path, std::shared_ptr<Shader> shader, bool flipUVs);

private:
    std::string directory;
    std::shared_ptr<Shader> shader;
    // std::unordered_map<std::string, std::unique_ptr<Texture>> texturesLoaded;
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    void LoadMaterial(aiMesh *mesh, const aiScene *scene);
    std::shared_ptr<Texture2D> LoadMaterialTexture(aiString str);
};

#endif
