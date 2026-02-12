#include "model.hpp"
#include "texture.hpp"

void Model::Draw(unsigned int shader)
{
    for (auto &mesh : meshes)
	mesh.Draw(shader);
}

// Entry point to model loading
void Model::LoadModel(std::string path, bool flipUVs)
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
Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    // Create corresponding vectors for this mesh
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Load all verticies for this mesh
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
	// Load vertex
	Vertex vertex;
	glm::vec3 tmp;
	tmp.x = mesh->mVertices[i].x;
	tmp.y = mesh->mVertices[i].y;
	tmp.z = mesh->mVertices[i].z;
	vertex.position = tmp;

	tmp.x = mesh->mNormals[i].x;
	tmp.y = mesh->mNormals[i].y;
	tmp.z = mesh->mNormals[i].z;
	vertex.normal = tmp;

	// Load texture coords if they are present
	if (mesh->mTextureCoords[0]) {
	    glm::vec2 vec;
	    vec.x = mesh->mTextureCoords[0][i].x;
	    vec.y = mesh->mTextureCoords[0][i].y;
	    vertex.texCoord = vec;
	}
	else
	    vertex.texCoord = glm::vec2(0.0f, 0.0f);

	vertices.push_back(vertex);
    }

    // Load indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
	for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
	{
	    indices.push_back(mesh->mFaces[i].mIndices[j]);
	}
    }


    // Get material by its index
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    
    // Texture filename gets loaded here
    aiString str;

    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &str) == AI_SUCCESS) {
	// Load this type of texture
	Texture diffuseMap = LoadMaterialTextures(str, aiTextureType_DIFFUSE);
	// And push it to the vector
	textures.push_back(diffuseMap);
    }
    else {
	printf("No diffuse texture for %s\n", directory.c_str());
    }

    if (material->GetTexture(aiTextureType_SPECULAR, 0, &str) == AI_SUCCESS) {
	Texture specularMap = LoadMaterialTextures(str, aiTextureType_SPECULAR);
	textures.push_back(specularMap);
    }
    else {
	printf("No specular texture for %s\n", directory.c_str());
    }



	//    // Some debug info
	//    printf (" == Mesh loaded");
	//    printf ("Textures:\n");
	//    int i = 1;
	//    for (auto t : textures) {
	// printf ("%d: %s (%s)\n", i, t.path.c_str(), t.type == TextureType::diffuse ? "diffuse" : "specular");
	// i++;
	// }

    return Mesh(vertices, indices, textures);
}

Texture Model::LoadMaterialTextures(aiString str, aiTextureType type)
{
    // get full path for this texture (not just the file name)
    std::string path = directory + "/" + str.C_Str();

    // check all textures that have been loaded
    for (unsigned int j = 0; j < texturesLoaded.size(); j++)
    {
	// if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
	// compare its path to the one we're loading right now
	if (std::strcmp(texturesLoaded[j].path.data(), path.data()) == 0)
	{
	    // if it is the same, return it
	    return texturesLoaded[j];
	}
    }
    
    // Create a new texture
    Texture texture;
    printf("Loading %s texture %s for material\n", type == aiTextureType_DIFFUSE ? "diffuse" : "specular", path.c_str());
    // Load texture data from file into gpu memory
    texture.id = CreateTexture(path.c_str());
    // Set texutre type
    texture.type = type == aiTextureType_DIFFUSE ? TextureType::diffuse : TextureType::specular;
    // Set texture path so we can compare later
    texture.path = path.c_str();
    // Add to textures that are already loaded
    texturesLoaded.push_back(texture);
    
    return texture;
}

const char* Model::GetDirectory()
{
    return directory.c_str();
}
