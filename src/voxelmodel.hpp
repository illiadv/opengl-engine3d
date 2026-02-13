#ifndef VOXELMODEL_HPP
#define VOXELMODEL_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "basemodel.hpp"
#include "mesh.hpp"

struct VoxelArray;

class VoxelModel : public BaseModel
{
    public:
	VoxelModel(const char* path);
	void Draw(unsigned int shader);
	const char* GetDirectory();

    private:
	void LoadVoxels(const char* path, VoxelArray& array);
	Mesh Mesher(VoxelArray& array);
	std::string directory;
	std::vector<glm::vec3> verts;
	std::vector<Mesh> meshes;
};

#endif
