#ifndef VOXELMODEL_HPP
#define VOXELMODEL_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "basemodel.hpp"
#include "mesh.hpp"

struct VoxelArray
{
    int width;
    int height;
    int depth;
    std::vector<unsigned char> data;

    VoxelArray(){}
    VoxelArray(int width, int height, int depth)
	: width(width), height(height), depth(depth)
    {
	data = std::vector<unsigned char>(width * height * depth);
    }
};

class GreedyMesher {
private:
    VoxelArray array;
    VoxelArray visitedTop;
    VoxelArray visitedBottom;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
public:
    GreedyMesher(VoxelArray a)
    {
	array = a;
	visitedTop = VoxelArray(array.width, array.height, array.depth);
	visitedBottom = VoxelArray(array.width, array.height, array.depth);
    }
    // void SetVisitedTop(int x, int y, int z);
    bool FaceExistsBottom(int x, int y, int z);
    bool RowFullBottomX(int x1, int x2, int y, int z);
    void CreateFaceBottom(int x, int y, int z);

    bool FaceExistsTop(int x, int y, int z);
    bool RowFullTopX(int x1, int x2, int y, int z);
    void CreateFaceTop(int x, int y, int z);
    Mesh CreateMesh();

};

class VoxelModel : public BaseModel
{
    public:
	VoxelModel(const char* path);
	void Draw(unsigned int shader);
	const char* GetDirectory();
	void LoadModel(const char* path);

    private:
	void LoadVoxels(const char* path, VoxelArray& array);
	Mesh Mesher(VoxelArray& array);
	// Mesh GreedyMesher(VoxelArray& array);
	std::string directory;
	std::vector<glm::vec3> verts;
	std::vector<Mesh> meshes;
};

#endif
