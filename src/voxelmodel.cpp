#include "voxelmodel.hpp"
#include <vector>
#include <fstream>
#include "mesh.hpp"
#include "texture.hpp"

struct VoxelArray
{
    int width;
    int height;
    int depth;
    std::vector<char> data;
};

int Get(int x, int y, int z, VoxelArray* va)
{
    if (x >= va->width || y >= va->height || z >= va->depth ||
	    x < 0 || y < 0 || z < 0)
	return 0;

    return va->data[y * va->width * va->depth + z * va->width + x];
}

VoxelModel::VoxelModel(const char* path)
{
    directory = path;
    VoxelArray array;
    LoadVoxels(path, array);
    meshes.push_back(Mesher(array));
}

void MarchingCubes(VoxelArray& array, std::vector<glm::vec3>& verts)
{
    for (int y = -1; y < (array.height + 1); y++)
    {
	printf("balls\n");
	for (int z = -1; z < (array.depth + 1); z++)
	{
	    for (int x = -1; x < (array.width + 1); x++)
	    {
		unsigned char sum = 
		    Get(x, y, z, &array) +
		    Get(x, y, z+1, &array) +
		    Get(x, y+1, z, &array) +
		    Get(x, y+1, z+1, &array) +
		    Get(x+1, y, z, &array) +
		    Get(x+1, y, z+1, &array) +
		    Get(x+1, y+1, z, &array) +
		    Get(x+1, y+1, z+1, &array);

		if (sum == 1 || sum == 7)
		{
		    verts.push_back(glm::vec3(x+1, y+1, z+1));
		}
	    }
	}
    }
}

Mesh VoxelModel::Mesher(VoxelArray& array)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int y = 0; y < (array.height); y++)
    {
	for (int z = 0; z < (array.depth); z++)
	{
	    for (int x = 0; x < (array.width); x++)
	    {
		if (!Get(x, y, z, &array))
		    continue;
		
		// top
		if (!Get(x, y-1, z, &array)) {
		    // Triangle 1
		    vertices.push_back(Vertex{{x, y, z+1}, {0, 1, 0}, {1, 0}});
		    vertices.push_back(Vertex{{x, y, z}, {0, 1, 0}, {0, 0}});
		    vertices.push_back(Vertex{{x+1, y, z}, {0, 1, 0}, {0, 1}});
		    // Triangle 2
		    vertices.push_back(Vertex{{x+1, y, z}, {0, 1, 0}, {0, 1}});
		    vertices.push_back(Vertex{{x+1, y, z+1}, {0, 1, 0}, {1, 1}});
		    vertices.push_back(Vertex{{x, y, z+1}, {0, 1, 0}, {1, 0}});
		}

		// bottom
		if (!Get(x, y+1, z, &array)) {
		    // Triangle 1
		    vertices.push_back(Vertex{{x, y+1, z+1}, {0, 1, 0}, {1, 0}});
		    vertices.push_back(Vertex{{x, y+1, z}, {0, 1, 0}, {0, 0}});
		    vertices.push_back(Vertex{{x+1, y+1, z}, {0, 1, 0}, {0, 1}});
		    // Triangle 2
		    vertices.push_back(Vertex{{x+1, y+1, z}, {0, 1, 0}, {0, 1}});
		    vertices.push_back(Vertex{{x+1, y+1, z+1}, {0, 1, 0}, {1, 1}});
		    vertices.push_back(Vertex{{x, y+1, z+1}, {0, 1, 0}, {1, 0}});
		}

		// left
		if (!Get(x-1, y, z, &array)) {
		    vertices.push_back(Vertex{{x, y+1, z}, {-1, 0, 0}, {1, 0}});
		    vertices.push_back(Vertex{{x, y, z}, {-1, 0, 0}, {0, 0}});
		    vertices.push_back(Vertex{{x, y, z+1}, {-1, 0, 0}, {0, 1}});

		    vertices.push_back(Vertex{{x, y, z+1}, {-1, 0, 0}, {0, 1}});
		    vertices.push_back(Vertex{{x, y+1, z+1}, {-1, 0, 0}, {1, 1}});
		    vertices.push_back(Vertex{{x, y+1, z}, {-1, 0, 0}, {1, 0}});
		}
		// right
		if (!Get(x+1, y, z, &array)) {
		    vertices.push_back(Vertex{{x+1, y+1, z}, {1, 0, 0}, {1, 0}});
		    vertices.push_back(Vertex{{x+1, y, z}, {1, 0, 0}, {0, 0}});
		    vertices.push_back(Vertex{{x+1, y, z+1}, {1, 0, 0}, {0, 1}});

		    vertices.push_back(Vertex{{x+1, y, z+1}, {1, 0, 0}, {0, 1}});
		    vertices.push_back(Vertex{{x+1, y+1, z+1}, {1, 0, 0}, {1, 1}});
		    vertices.push_back(Vertex{{x+1, y+1, z}, {1, 0, 0}, {1, 0}});
		}

		// back
		if (!Get(x, y, z-1, &array)) {
		    vertices.push_back(Vertex{{x, y+1, z}, {0, 0, -1}, {1, 0}});
		    vertices.push_back(Vertex{{x, y, z}, {0, 0, -1}, {0, 0}});
		    vertices.push_back(Vertex{{x+1, y, z}, {0, 0, -1}, {0, 1}});

		    vertices.push_back(Vertex{{x+1, y, z}, {0, 0, -1}, {0, 1}});
		    vertices.push_back(Vertex{{x+1, y+1, z}, {0, 0, -1}, {1, 1}});
		    vertices.push_back(Vertex{{x, y+1, z}, {0, 0, -1}, {1, 0}});
		}
		// front
		if (!Get(x, y, z+1, &array)) {
		    vertices.push_back(Vertex{{x, y+1, z+1}, {0, 0, -1}, {1, 0}});
		    vertices.push_back(Vertex{{x, y, z+1}, {0, 0, -1}, {0, 0}});
		    vertices.push_back(Vertex{{x+1, y, z+1}, {0, 0, -1}, {0, 1}});

		    vertices.push_back(Vertex{{x+1, y, z+1}, {0, 0, -1}, {0, 1}});
		    vertices.push_back(Vertex{{x+1, y+1, z+1}, {0, 0, -1}, {1, 1}});
		    vertices.push_back(Vertex{{x, y+1, z+1}, {0, 0, -1}, {1, 0}});
		}
	    }
	}
    }

    indices.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++)
	indices[i] = i;
    
    std::vector<Texture> textures{};
    Texture tex;
    tex.id = CreateTexture("./assets/crate/container2.png");
    tex.type = TextureType::diffuse;
    textures.push_back(tex);

    printf("Mesher finished; %zu verts, %zu tris\n", vertices.size(), indices.size() / 3);

    Mesh mesh(vertices, indices, textures);
    return mesh;
}

const char* VoxelModel::GetDirectory()
{
    return directory.c_str();
}


void VoxelModel::LoadVoxels(const char* path, VoxelArray& array)
{
    std::fstream f(path); 
    if (!f.is_open())
    {
	printf("Failed to open %s\n", path);
	std::terminate();
    }

    f >> array.width;
    f >> array.height;
    f >> array.depth;

    array.data = std::vector<char>(array.width * array.height * array.depth);

    for (size_t i = 0; i < array.data.size(); i++)
    {
	char c;
	f >> c;
	array.data[i] = (c == '#' ? 1 : 0);
    }
    
    // f.close()

}

void VoxelModel::Draw(unsigned int shader)
{
    for (auto v : verts) {
	std::vector<Vertex> vertices = {
		// positions          // normals           // texture coords
	    Vertex{{-0.05f, -0.05f, -0.05f},  {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f}},
	    Vertex{   { 0.05f, -0.05f, -0.05f},  {0.0f,  0.0f, -1.0f},  {1.0f,  0.0f}},
	    Vertex{   { 0.05f,  0.05f, -0.05f},  {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f}},
	    Vertex{   { 0.05f,  0.05f, -0.05f},  {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f}},
	    Vertex{   {-0.05f,  0.05f, -0.05f},  {0.0f,  0.0f, -1.0f},  {0.0f,  1.0f}},
	    Vertex{   {-0.05f, -0.05f, -0.05f},  {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f}},
				   
	     Vertex{  {-0.05f, -0.05f,  0.05f},  {0.0f,  0.0f,  1.0f},  {0.0f,  0.0f}},
	     Vertex{  { 0.05f, -0.05f,  0.05f},  {0.0f,  0.0f,  1.0f},  {1.0f,  0.0f}},
	     Vertex{  { 0.05f,  0.05f,  0.05f},  {0.0f,  0.0f,  1.0f},  {1.0f,  1.0f}},
	     Vertex{  { 0.05f,  0.05f,  0.05f},  {0.0f,  0.0f,  1.0f},  {1.0f,  1.0f}},
	     Vertex{  {-0.05f,  0.05f,  0.05f},  {0.0f,  0.0f,  1.0f},  {0.0f,  1.0f}},
	     Vertex{  {-0.05f, -0.05f,  0.05f},  {0.0f,  0.0f,  1.0f},  {0.0f,  0.0f}},
				   
	     Vertex { {-0.05f,  0.05f,  0.05f}, {-1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
	     Vertex { {-0.05f,  0.05f, -0.05f}, {-1.0f,  0.0f,  0.0f},  {1.0f,  1.0f}},
	     Vertex { {-0.05f, -0.05f, -0.05f}, {-1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
	     Vertex { {-0.05f, -0.05f, -0.05f}, {-1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
	     Vertex { {-0.05f, -0.05f,  0.05f}, {-1.0f,  0.0f,  0.0f},  {0.0f,  0.0f}},
	     Vertex { {-0.05f,  0.05f,  0.05f}, {-1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
				   
	     Vertex  {{ 0.05f,  0.05f,  0.05f},  {1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
	     Vertex  {{ 0.05f,  0.05f, -0.05f},  {1.0f,  0.0f,  0.0f},  {1.0f,  1.0f}},
	     Vertex  {{ 0.05f, -0.05f, -0.05f},  {1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
	     Vertex  {{ 0.05f, -0.05f, -0.05f},  {1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
	     Vertex  {{ 0.05f, -0.05f,  0.05f},  {1.0f,  0.0f,  0.0f},  {0.0f,  0.0f}},
	     Vertex  {{ 0.05f,  0.05f,  0.05f},  {1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
				   
	     Vertex  {{-0.05f, -0.05f, -0.05f},  {0.0f, -1.0f,  0.0f},  {0.0f,  1.0f}},
	     Vertex  {{ 0.05f, -0.05f, -0.05f},  {0.0f, -1.0f,  0.0f},  {1.0f,  1.0f}},
	     Vertex  {{ 0.05f, -0.05f,  0.05f},  {0.0f, -1.0f,  0.0f},  {1.0f,  0.0f}},
	     Vertex  {{ 0.05f, -0.05f,  0.05f},  {0.0f, -1.0f,  0.0f},  {1.0f,  0.0f}},
	     Vertex  {{-0.05f, -0.05f,  0.05f},  {0.0f, -1.0f,  0.0f},  {0.0f,  0.0f}},
	     Vertex  {{-0.05f, -0.05f, -0.05f},  {0.0f, -1.0f,  0.0f},  {0.0f,  1.0f}},
				   
	     Vertex  {{-0.05f,  0.05f, -0.05f},  {0.0f,  1.0f,  0.0f},  {0.0f,  1.0f}},
	     Vertex  {{ 0.05f,  0.05f, -0.05f},  {0.0f,  1.0f,  0.0f},  {1.0f,  1.0f}},
	     Vertex  {{ 0.05f,  0.05f,  0.05f},  {0.0f,  1.0f,  0.0f},  {1.0f,  0.0f}},
	     Vertex  {{ 0.05f,  0.05f,  0.05f},  {0.0f,  1.0f,  0.0f},  {1.0f,  0.0f}},
	     Vertex  {{-0.05f,  0.05f,  0.05f},  {0.0f,  1.0f,  0.0f},  {0.0f,  0.0f}},
	     Vertex  {{-0.05f,  0.05f, -0.05f},  {0.0f,  1.0f,  0.0f},  {0.0f,  1.0f}}
	    };

	std::vector<unsigned int> indices(36);
	for (int i = 0; i < 36; i++) {
	    indices[i] = i;
	    vertices[i].position.x += v.x;
	    vertices[i].position.y += v.y;
	    vertices[i].position.z += v.z;
	}
	std::vector<Texture> textures{};

	Mesh mesh(vertices, indices, textures);

	mesh.Draw(shader);
    }

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (auto m : meshes)
    {
	m.Draw(shader);
    }
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
