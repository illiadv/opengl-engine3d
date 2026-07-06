#include "engine3d/model.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};


// Entry point to model loading
Model::Model(std::string path, std::shared_ptr<Shader> shader, bool flipUVs)
    : material(shader), shader(shader)
{
    Assimp::Importer importer;

    unsigned int flags = 0;
    flags |= aiProcess_Triangulate;

    if (flipUVs) {
	flags |= aiProcess_FlipUVs;
    }

    // Load the scene
    const aiScene *scene = importer.ReadFile(path.c_str(), flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
	printf("Assimp error: %s", importer.GetErrorString());
	return;
    }

    // Get directory path where the model is located
    directory = path.substr(0, path.find_last_of('/'));

    // Process the root node
    ProcessNode(scene->mRootNode, scene);

}

// Process nodes recursively
void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    // nodes only contain mesh indexes
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
	// Get a mesh by its index
	aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
	// Process a mesh (load its data) and add it to model's meshes
	meshes.push_back(ProcessMesh(mesh, scene));
    }

    // After that, process all children nodes of this mesh recursively
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
	ProcessNode(node->mChildren[i], scene);
    }
}

// Called from ProcessNode
Mesh Model::ProcessMesh(aiMesh *assimpMesh, const aiScene *scene)
{
    // Create corresponding vectors for this mesh
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    // std::vector<Texture> textures;

    // Load all verticies for this mesh
    for (unsigned int i = 0; i < assimpMesh->mNumVertices; i++)
    {
	// Load vertex
	Vertex vertex;
	glm::vec3 tmp;
	tmp.x = assimpMesh->mVertices[i].x;
	tmp.y = assimpMesh->mVertices[i].y;
	tmp.z = assimpMesh->mVertices[i].z;
	vertex.position = tmp;

	tmp.x = assimpMesh->mNormals[i].x;
	tmp.y = assimpMesh->mNormals[i].y;
	tmp.z = assimpMesh->mNormals[i].z;
	vertex.normal = tmp;

	// Load texture coords if they are present
	if (assimpMesh->mTextureCoords[0]) {
	    glm::vec2 vec;
	    vec.x = assimpMesh->mTextureCoords[0][i].x;
	    vec.y = assimpMesh->mTextureCoords[0][i].y;
	    vertex.texCoord = vec;
	}
	else
	    vertex.texCoord = glm::vec2(0.0f, 0.0f);

	vertices.push_back(vertex);
    }

    // Load indices
    for (unsigned int i = 0; i < assimpMesh->mNumFaces; i++)
    {
	for (unsigned int j = 0; j < assimpMesh->mFaces[i].mNumIndices; j++)
	{
	    indices.push_back(assimpMesh->mFaces[i].mIndices[j]);
	}
    }

    LoadMaterial(assimpMesh, scene);

    BufferLayout layout = {
	BufferElement( AttributeType::Float, 3 ), // Position
	BufferElement( AttributeType::Float, 3 ), // Normal
	BufferElement( AttributeType::Float, 2 ), // TexCoord
    };

    return Mesh(vertices.data(), vertices.size() * sizeof(Vertex), indices, layout);
}

void Model::LoadMaterial(aiMesh *assimpMesh, const aiScene *assimpScene)
{
    int materialIndex = assimpMesh->mMaterialIndex;
    if (materialIndex <= materialIndexLoaded)
    {
	return; // No need to load this material again
    }
    // Get material by its index
    aiMaterial *assimpMaterial = assimpScene->mMaterials[materialIndex];
    materialIndexLoaded++;

    aiString textureFilename;

    if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilename) == AI_SUCCESS) {
	auto texture = LoadMaterialTexture(textureFilename);
        material.SetTexture("material.diffuse", texture, 0);
    }
    else {
        printf("No diffuse texture for %s\n", directory.c_str());
    }

    if (assimpMaterial->GetTexture(aiTextureType_SPECULAR, 0, &textureFilename) == AI_SUCCESS) {
	auto texture = LoadMaterialTexture(textureFilename);
        material.SetTexture("material.specular", texture, 1);
    }
    else {
        printf("No specular texture for %s\n", directory.c_str());
    }

    material.SetFloat("material.shininess", 64);
}


std::shared_ptr<Texture2D> Model::LoadMaterialTexture(aiString str)
{
    // get full path for this texture (not just the file name)
    std::string path = directory + "/" + str.C_Str();

    // Create a new texture
    return std::make_shared<Texture2D>(path.c_str());
}
