#include "voxelmodel.hpp"
#include <vector>
#include <fstream>
#include "mesh.hpp"
#include "texture.hpp"


int Get(int x, int y, int z, VoxelArray* va)
{
    if (x >= va->width || y >= va->height || z >= va->depth ||
	    x < 0 || y < 0 || z < 0)
	return 0;

    return va->data[y * va->width * va->depth + z * va->width + x];
}

void Set(int x, int y, int z, VoxelArray* va, unsigned char value)
{
    if (x >= va->width || y >= va->height || z >= va->depth ||
	    x < 0 || y < 0 || z < 0)
    {
	printf ("Oh no!!\n\n\n");
    }
    va->data[y * va->width * va->depth + z * va->width + x] = value;
}


VoxelModel::VoxelModel(const char* path)
{
    LoadModel(path);
}

void VoxelModel::LoadModel(const char* path)
{
    meshes.clear();
    directory = path;
    VoxelArray array;
    LoadVoxels(path, array);
    GreedyMesher mesher(array);
    meshes.push_back(mesher.CreateMesh());
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

void GreedyMesher::AddFace(Face face)
{
    glm::vec3 normal{};
    int u;
    int v;

    switch (face.direction) {
	case FACE_BOTTOM: normal = {0, -1, 0};
		       u = face.bottomRight.x - face.bottomLeft.x;
		       v = face.topLeft.z - face.bottomLeft.z;
		       break;
	case FACE_TOP: normal = {0, 1, 0};
		       u = face.bottomRight.x - face.bottomLeft.x;
		       v = face.bottomLeft.z - face.topLeft.z;
		       break;
	case FACE_LEFT: normal = {-1, 0, 0};
		       u = face.bottomRight.z - face.bottomLeft.z;
		       v = face.topLeft.y - face.bottomLeft.y;
		       break;
	case FACE_RIGHT: normal = {1, 0, 0};
		       u = face.bottomLeft.z - face.bottomRight.z;
		       v = face.topLeft.y - face.bottomLeft.y;
		       break;
	case FACE_BACK: normal = {0, 0, -1};
		       u = face.bottomLeft.x - face.bottomRight.x;
		       v = face.topLeft.y - face.bottomLeft.y;
		       break;
	case FACE_FRONT: normal = {0, 0, 1};
		       u = face.bottomRight.x - face.bottomLeft.x;
		       v = face.topLeft.y - face.bottomLeft.y;
		       break;
    
    }

    vertices.push_back(Vertex{face.bottomLeft, normal, {0, 0}});
    vertices.push_back(Vertex{face.bottomRight, normal, {u, 0}});
    vertices.push_back(Vertex{face.topRight, normal, {u, v}});
    vertices.push_back(Vertex{face.topLeft, normal, {0, v}});

    indices.push_back(currentIndex + 0);
    indices.push_back(currentIndex + 1);
    indices.push_back(currentIndex + 2);
    indices.push_back(currentIndex + 0);
    indices.push_back(currentIndex + 2);
    indices.push_back(currentIndex + 3);

    currentIndex += 4;
}

bool GreedyMesher::FaceExistsBottom(int x, int y, int z)
{
    return !Get(x, y, z, &visitedBottom) 
	&&  Get(x, y, z, &array) 
	&& !Get(x, y-1, z, &array);
}

bool GreedyMesher::FaceExistsTop(int x, int y, int z)
{
    return !Get(x, y, z, &visitedTop) 
	&&  Get(x, y, z, &array) 
	&& !Get(x, y+1, z, &array);
}

bool GreedyMesher::FaceExistsLeft(int x, int y, int z)
{
    return !Get(x, y, z, &visitedLeft) 
	&&  Get(x, y, z, &array) 
	&& !Get(x-1, y, z, &array);
}

bool GreedyMesher::FaceExistsRight(int x, int y, int z)
{
    return !Get(x, y, z, &visitedRight) 
	&&  Get(x, y, z, &array) 
	&& !Get(x+1, y, z, &array);
}

bool GreedyMesher::FaceExistsBack(int x, int y, int z)
{
    return !Get(x, y, z, &visitedBack) 
	&&  Get(x, y, z, &array) 
	&& !Get(x, y, z-1, &array);
}

bool GreedyMesher::FaceExistsFront(int x, int y, int z)
{
    return !Get(x, y, z, &visitedFront) 
	&&  Get(x, y, z, &array) 
	&& !Get(x, y, z+1, &array);
}

bool GreedyMesher::RowFullBottomX(int x1, int x2, int y, int z)
{
    for (int i = x1; i <= x2; i++)
    {
	if (!FaceExistsBottom(i, y, z)) {
	    return false;
	}
    }
    for (int i = x1; i <= x2; i++)
    {
	// Put into visited
	Set(i, y, z, &visitedBottom, 1);
    }
    return true;
}

bool GreedyMesher::RowFullTopX(int x1, int x2, int y, int z)
{
    for (int i = x1; i <= x2; i++)
    {
	if (!FaceExistsTop(i, y, z)) {
	    return false;
	}
    }
    for (int i = x1; i <= x2; i++)
    {
	// Put into visited
	Set(i, y, z, &visitedTop, 1);
    }
    return true;
}

bool GreedyMesher::RowFullLeftY(int x, int y1, int y2, int z)
{
    for (int i = y1; i <= y2; i++)
    {
	if (!FaceExistsLeft(x, i, z)) {
	    return false;
	}
    }
    for (int i = y1; i <= y2; i++)
    {
	// Put into visited
	Set(x, i, z, &visitedLeft, 1);
    }
    return true;
}

bool GreedyMesher::RowFullRightY(int x, int y1, int y2, int z)
{
    for (int i = y1; i <= y2; i++)
    {
	if (!FaceExistsRight(x, i, z)) {
	    return false;
	}
    }
    for (int i = y1; i <= y2; i++)
    {
	// Put into visited
	Set(x, i, z, &visitedRight, 1);
    }
    return true;
}

bool GreedyMesher::RowFullBackY(int x, int y1, int y2, int z)
{
    for (int i = y1; i <= y2; i++)
    {
	if (!FaceExistsBack(x, i, z)) {
	    return false;
	}
    }
    for (int i = y1; i <= y2; i++)
    {
	// Put into visited
	Set(x, i, z, &visitedBack, 1);
    }
    return true;
}

bool GreedyMesher::RowFullFrontY(int x, int y1, int y2, int z)
{
    for (int i = y1; i <= y2; i++)
    {
	if (!FaceExistsFront(x, i, z)) {
	    return false;
	}
    }
    for (int i = y1; i <= y2; i++)
    {
	// Put into visited
	Set(x, i, z, &visitedFront, 1);
    }
    return true;
}

void GreedyMesher::CreateFaceBottom(int x, int y, int z)
{
    if (!FaceExistsBottom(x, y, z)) {
	return;
    }

    // Put into visited
    Set(x, y, z, &visitedBottom, 1);

    // Check X direction:
    int xi = x;
    while (1) {
	xi++;
	if (!FaceExistsBottom(xi, y, z)) {
	    break;
	}

	// Put into visited
	Set(xi, y, z, &visitedBottom, 1);
    }

    int xf = xi-1;

    // Check Z direction
    int zi = z;

    while (1) {
	zi++;
	if (!RowFullBottomX(x, xf, y, zi))
	{
	    break;
	}
    }

    Face face;

    face.bottomLeft  = {x,  y, z};
    face.bottomRight = {xi, y, z};
    face.topRight    = {xi, y, zi};
    face.topLeft     = {x,  y, zi};
    face.direction = FACE_BOTTOM;

    AddFace(face);
}

void GreedyMesher::CreateFaceTop(int x, int y, int z)
{
    if (!FaceExistsTop(x, y, z)) {
	return;
    }

    // Put into visited
    Set(x, y, z, &visitedTop, 1);

    // Check X direction:
    int xi = x;
    while (1) {
	xi++;
	if (!FaceExistsTop(xi, y, z)) {
	    break;
	}

	// Put into visited
	Set(xi, y, z, &visitedTop, 1);
    }

    int xf = xi-1;

    // Check Z direction
    int zi = z;

    while (1) {
	zi++;
	if (!RowFullTopX(x, xf, y, zi))
	{
	    break;
	}
    }

    Face face;

    face.bottomLeft  = {x,  y+1, zi};
    face.bottomRight = {xi, y+1, zi};
    face.topRight    = {xi, y+1, z};
    face.topLeft     = {x,  y+1, z};
    face.direction = FACE_TOP;

    AddFace(face);

    // int zf = zi-1;

}

void GreedyMesher::CreateFaceLeft(int x, int y, int z)
{
    if (!FaceExistsLeft(x, y, z)) {
	return;
    }

    // Put into visited
    Set(x, y, z, &visitedLeft, 1);

    // Check Y direction:
    int yi = y;
    while (1) {
	yi++;
	if (!FaceExistsLeft(x, yi, z)) {
	    break;
	}

	// Put into visited
	Set(x, yi, z, &visitedLeft, 1);
    }

    int yf = yi-1;

    // Check Z direction
    int zi = z;

    while (1) {
	zi++;
	if (!RowFullLeftY(x, y, yf, zi))
	{
	    break;
	}
    }

    Face face;

    face.bottomLeft  = {x, y, z};
    face.bottomRight = {x, y, zi};
    face.topRight    = {x, yi, zi};
    face.topLeft     = {x, yi, z};
    face.direction = FACE_LEFT;

    AddFace(face);
}

void GreedyMesher::CreateFaceRight(int x, int y, int z)
{
    if (!FaceExistsRight(x, y, z)) {
	return;
    }

    // Put into visited
    Set(x, y, z, &visitedRight, 1);

    // Check Y direction:
    int yi = y;
    while (1) {
	yi++;
	if (!FaceExistsRight(x, yi, z)) {
	    break;
	}

	// Put into visited
	Set(x, yi, z, &visitedRight, 1);
    }

    int yf = yi-1;

    // Check Z direction
    int zi = z;

    while (1) {
	zi++;
	if (!RowFullRightY(x, y, yf, zi))
	{
	    break;
	}
    }

    Face face;

    face.bottomLeft  = {x + 1, y, zi};
    face.bottomRight = {x + 1, y, z};
    face.topRight    = {x + 1, yi, z};
    face.topLeft     = {x + 1, yi, zi};
    face.direction = FACE_RIGHT;

    AddFace(face);
}

void GreedyMesher::CreateFaceBack(int x, int y, int z)
{
    if (!FaceExistsBack(x, y, z)) {
	return;
    }

    // Put into visited
    Set(x, y, z, &visitedBack, 1);

    // Check Y direction:
    int yi = y;
    while (1) {
	yi++;
	if (!FaceExistsBack(x, yi, z)) {
	    break;
	}

	// Put into visited
	Set(x, yi, z, &visitedBack, 1);
    }

    int yf = yi-1;

    // Check X direction
    int xi = x;

    while (1) {
	xi++;
	if (!RowFullBackY(xi, y, yf, z))
	{
	    break;
	}
    }

    Face face;

    face.bottomLeft  = {xi, y, z};
    face.bottomRight = {x, y, z};
    face.topRight    = {x, yi, z};
    face.topLeft     = {xi, yi, z};
    face.direction = FACE_BACK;

    AddFace(face);
}

void GreedyMesher::CreateFaceFront(int x, int y, int z)
{
    if (!FaceExistsFront(x, y, z)) {
	return;
    }

    // Put into visited
    Set(x, y, z, &visitedFront, 1);

    // Check Y direction:
    int yi = y;
    while (1) {
	yi++;
	if (!FaceExistsFront(x, yi, z)) {
	    break;
	}

	// Put into visited
	Set(x, yi, z, &visitedFront, 1);
    }

    int yf = yi-1;

    // Check X direction
    int xi = x;

    while (1) {
	xi++;
	if (!RowFullFrontY(xi, y, yf, z))
	{
	    break;
	}
    }

    Face face;

    face.bottomLeft  = {x, y, z+1};
    face.bottomRight = {xi, y, z+1};
    face.topRight    = {xi, yi, z+1};
    face.topLeft     = {x, yi, z+1};
    face.direction = FACE_FRONT;

    AddFace(face);


}

Mesh GreedyMesher::CreateMesh()
{

    for (int y = 0; y < (array.height); y++)
    {
	for (int z = 0; z < (array.depth); z++)
	{
	    for (int x = 0; x < (array.width); x++)
	    {
		CreateFaceBottom(x, y, z);
		CreateFaceTop(x, y, z);
		CreateFaceLeft(x, y, z);
		CreateFaceRight(x, y, z);
		CreateFaceBack(x, y, z);
		CreateFaceFront(x, y, z);

	    }
	}
    }

    std::vector<Texture> textures{};
    Texture tex;
    tex.id = CreateTexture("./assets/test.png");
    tex.type = TextureType::diffuse;
    textures.push_back(tex);

    printf("Mesher finished; %zu verts, %zu tris\n", vertices.size(), indices.size() / 3);

    Mesh mesh(vertices, indices, textures);
    return mesh;
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

    // array.data = std::vector<char>(array.width * array.height * array.depth);
    array = VoxelArray(array.width, array.height, array.depth);

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
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (auto m : meshes)
    {
	m.Draw(shader);
    }
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
