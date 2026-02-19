#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"
#include "LoadObj.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateQuad(const std::string& meshName, glm::vec3 color, float length = 1.f);
	static Mesh* GenerateCube(const std::string& meshName, glm::vec3 color, float length = 1.f);
	static Mesh* GenerateCuboid(const std::string& meshName, glm::vec3 color, float lengthX = 1.f, float lengthY = 1.f, float lengthZ = 1.f);
	static Mesh* GenerateSphere(const std::string& meshName, glm::vec3 color, float radius = 1.f, int numSlice = 360, int numStack = 360);
	static Mesh* GenerateCylinder(const std::string& meshName, glm::vec3 color, unsigned numSlice, float radius, float height);
	static Mesh* GenerateHemisphere(const std::string& meshName, glm::vec3 color, unsigned numStack, unsigned numSlice, float radius);
	static Mesh* GenerateCone(const std::string& meshName, glm::vec3 color, unsigned numStack, unsigned numSlice, float radius, float height);
	static Mesh* GenerateOBJ(const std::string& meshName, const std::string& file_path);
	static Mesh* GenerateOBJMTL(const std::string& meshName, const std::string& file_path, const std::string& mtl_path);
	static Mesh* GenerateText(const std::string& meshName, unsigned numRow, unsigned numCol);
};

#endif


