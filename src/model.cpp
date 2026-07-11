#include "engine3d/model.hpp"
#include <stdexcept>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

// Entry point to model loading
Model::Model(std::string path, std::shared_ptr<Shader> shader, bool flipUVs)
    : shader(shader)
{
    Assimp::Importer importer;

    unsigned int assimpFlags = 0;
    assimpFlags |= aiProcess_Triangulate;

    if (flipUVs) {
	assimpFlags |= aiProcess_FlipUVs;
    }

    // Load the scene
    const aiScene *assimpScene = importer.ReadFile(path.c_str(), assimpFlags);

    if (!assimpScene || assimpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimpScene->mRootNode)
    {
	throw std::runtime_error(std::string("Assimp error: ") + importer.GetErrorString());
    }

    // Get directory path where the model is located
    directory = path.substr(0, path.find_last_of('/'));

    // Process the root node
    ProcessNode(assimpScene->mRootNode, assimpScene);

    for (unsigned int i = 0; i < assimpScene->mNumMaterials; i++)
    {
	aiMaterial *assimpMaterial = assimpScene->mMaterials[i];
	printf("Loading material for %s\n", path.c_str());
	LoadMaterial(assimpMaterial);
	printf("\n");
    }

}

// Process nodes recursively
void Model::ProcessNode(aiNode *assimpNode, const aiScene *assimpScene)
{
    // nodes only contain mesh indexes
    for (unsigned int i = 0; i < assimpNode->mNumMeshes; i++)
    {
	// Get a mesh by its index
	aiMesh *assimpMesh = assimpScene->mMeshes[assimpNode->mMeshes[i]];
	// Process a mesh (load its data) and add it to model's meshes
	meshes.push_back(ProcessMesh(assimpMesh));
    }

    // After that, process all children nodes of this mesh recursively
    for (unsigned int i = 0; i < assimpNode->mNumChildren; i++)
    {
	ProcessNode(assimpNode->mChildren[i], assimpScene);
    }
}

// Called from ProcessNode
Mesh Model::ProcessMesh(aiMesh *assimpMesh)
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

    // Store material index of this mesh to use when rendering
    materialIndexes.push_back(assimpMesh->mMaterialIndex);

    BufferLayout layout = {
	BufferElement( AttributeType::Float, 3 ), // Position
	BufferElement( AttributeType::Float, 3 ), // Normal
	BufferElement( AttributeType::Float, 2 ), // TexCoord
    };

    return Mesh(vertices.data(), vertices.size() * sizeof(Vertex), indices, layout);
}

void Model::LoadMaterial(aiMaterial *assimpMaterial)
{
    Material material(shader);

    aiString textureFilename;

    if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilename) == AI_SUCCESS) {
	auto texture = LoadMaterialTexture(textureFilename);
        material.SetTexture("material.diffuse", texture, 0);
    }

    if (assimpMaterial->GetTexture(aiTextureType_SPECULAR, 0, &textureFilename) == AI_SUCCESS) {
	auto texture = LoadMaterialTexture(textureFilename);
        material.SetTexture("material.specular", texture, 1);
    }

    LoadMaterialKeys(material, assimpMaterial);

    materials.push_back(material);
}

void Model::LoadMaterialKeys(Material &material, aiMaterial *assimpMaterial)
{
    printf("Property keys found:\n");
    for (unsigned int i = 0; i < assimpMaterial->mNumProperties; i++)
    {
	aiMaterialProperty *property = assimpMaterial->mProperties[i];
	printf("%d. %s\n", i, property->mKey.C_Str());
    }
    aiString name;
    if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_NAME, name))
    {
	printf("NAME = %s\n", name.C_Str());
    }

    aiColor3D color;
    if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color))
    {
	printf("COLOR_AMBIENT = %.3f, %.3f, %.3f\n", color.r, color.g, color.b);
    }

    if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
    {
	printf("COLOR_DIFFUSE = %.3f, %.3f, %.3f\n", color.r, color.g, color.b);
    }

    if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color))
    {
	printf("COLOR_SPECULAR = %.3f, %.3f, %.3f\n", color.r, color.g, color.b);
    }

    float fl;
    if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_REFLECTIVITY, fl))
    {
	printf("REFLECTIVITY = %.3f\n", fl);
    }

    if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_SHININESS, fl))
    {
	printf("SHININESS = %.3f\n", fl);
	material.SetFloat("material.shininess", fl);
    }

    if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, fl))
    {
	printf("SHININESS_STRENGTH = %.3f\n", fl);
    }

}

std::shared_ptr<Texture2D> Model::LoadMaterialTexture(aiString filename)
{
    // get full path for this texture (not just the file name)
    std::string path = directory + "/" + filename.C_Str();

    // Create a new texture
    return std::make_shared<Texture2D>(path.c_str());
}
