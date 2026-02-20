#define _USE_MATH_DEFINES
#include <cmath>

#include "MeshBuilder.h"
#include <GL\glew.h>
#include <vector>
#include "LoadOBJ.h"


/******************************************************************************/
/*!
\brief
Generate the vertices of a reference Axes; Use red for x-axis, green for y-axis, blue for z-axis
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - x-axis should start at -lengthX / 2 and end at lengthX / 2
\param lengthY - y-axis should start at -lengthY / 2 and end at lengthY / 2
\param lengthZ - z-axis should start at -lengthZ / 2 and end at lengthZ / 2

\return Pointer to mesh storing VBO/IBO of reference axes
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateAxes(const std::string& meshName, float
	lengthX, float lengthY, float lengthZ)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	//x-axis
	v.pos = glm::vec3(-lengthX, 0, 0); v.color = glm::vec3(1, 0, 0);
	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(lengthX, 0, 0); v.color = glm::vec3(1, 0, 0);
	vertex_buffer_data.push_back(v);

	//y-axis
	v.pos = glm::vec3(0, -lengthY, 0); v.color = glm::vec3(0, 1, 0);
	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0, lengthY, 0); v.color = glm::vec3(0, 1, 0);
	vertex_buffer_data.push_back(v);

	//z-axis
	v.pos = glm::vec3(0, 0, -lengthZ); v.color = glm::vec3(0, 0, 1);
	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0, 0, lengthZ); v.color = glm::vec3(0, 0, 1);
	vertex_buffer_data.push_back(v);

	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(5);


	Mesh *mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_LINES;

	return mesh;
}

/******************************************************************************/
/*!
\brief
Generate the vertices of a quad; Use random color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - width of quad
\param lengthY - height of quad

\return Pointer to mesh storing VBO/IBO of quad
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateQuad(const std::string& meshName, glm::vec3
	color, float length)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	
	// Add the vertices here
	v.color = color;
	v.normal = glm::vec3(0, 0, 1);

	v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.f);
	v.texCoord = glm::vec2(1.f, 0.f);
	vertex_buffer_data.push_back(v); //v0
	
	v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.f);
	v.texCoord = glm::vec2(1.f, 1.f);
	vertex_buffer_data.push_back(v); //v1
	
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.f);
	v.texCoord = glm::vec2(0.f, 1.f);
	vertex_buffer_data.push_back(v); //v2
	
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.f);
	v.texCoord = glm::vec2(0.f, 0.f);
	vertex_buffer_data.push_back(v); //v3

	//tri1
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	//tri2
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);

	// Create the new mesh
	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}
/******************************************************************************/
Mesh* MeshBuilder::GenerateSphere(const std::string& meshName,
	glm::vec3 color, float radius, int numSlice, int numStack)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	v.color = color;
	

	float degreePerStack = glm::pi<float>() / numStack;
	float degreePerSlice = glm::two_pi<float>() / numSlice;

	for (unsigned stack = 0; stack < numStack + 1; ++stack) //stack
	{
		float phi = -glm::half_pi<float>() + stack * degreePerStack;
		for (unsigned slice = 0; slice < numSlice + 1; ++slice) //slice
		{
			float theta = slice * degreePerSlice;
			v.pos = glm::vec3(radius * glm::cos(phi) * glm::cos(theta),
				radius * glm::sin(phi),
				radius * glm::cos(phi) * glm::sin(theta));
			
			
			// Vertex normal of spheres are usually the vertex
				// position with length of one
				v.normal = glm::vec3(glm::cos(phi) *
					glm::cos(theta), glm::sin(phi), glm::cos(phi) *
					glm::sin(theta));
			
            v.texCoord = glm::vec2((float)slice / numSlice, (float)stack / numStack);
			vertex_buffer_data.push_back(v);
		}
	}





	for (unsigned stack = 0; stack < numStack; ++stack)
	{
		for (unsigned slice = 0; slice < numSlice + 1; ++slice)
		{
			index_buffer_data.push_back(stack * (numSlice + 1) + slice); // fill the index to push
			index_buffer_data.push_back((stack + 1) * (numSlice + 1) + slice); // fill the index to push
		}
	}




	// Create the new mesh
	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;

}

/******************************************************************************/
Mesh* MeshBuilder::GenerateTriangularPrism(const std::string& meshName, glm::vec3 color,
    float base, float height, float depth)
{
    Vertex v;
    v.color = color;
    std::vector<Vertex> vertex_buffer_data;
    std::vector<GLuint> index_buffer_data;

    float halfBase = base * 0.5f;
    float halfDepth = depth * 0.5f;

    // Front face (triangle) - indices 0, 1, 2
    v.pos = glm::vec3(-halfBase, 0, halfDepth);
    v.normal = glm::vec3(0, 0, 1);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-left
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfBase, 0, halfDepth);
    v.normal = glm::vec3(0, 0, 1);
    v.texCoord = glm::vec2(1.f, 0.f);   // Bottom-right
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(0, height, halfDepth);
    v.normal = glm::vec3(0, 0, 1);
    v.texCoord = glm::vec2(0.5f, 1.f);  // Top-center
    vertex_buffer_data.push_back(v);

    // Back face (triangle) - indices 3, 4, 5
    v.pos = glm::vec3(-halfBase, 0, -halfDepth);
    v.normal = glm::vec3(0, 0, -1);
    v.texCoord = glm::vec2(1.f, 0.f);   // Bottom-left (mirrored)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(0, height, -halfDepth);
    v.normal = glm::vec3(0, 0, -1);
    v.texCoord = glm::vec2(0.5f, 1.f);  // Top-center
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfBase, 0, -halfDepth);
    v.normal = glm::vec3(0, 0, -1);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-right (mirrored)
    vertex_buffer_data.push_back(v);

    // Bottom face (rectangle) - indices 6, 7, 8, 9
    v.pos = glm::vec3(-halfBase, 0, halfDepth);
    v.normal = glm::vec3(0, -1, 0);
    v.texCoord = glm::vec2(0.f, 1.f);   // Front-left
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfBase, 0, -halfDepth);
    v.normal = glm::vec3(0, -1, 0);
    v.texCoord = glm::vec2(0.f, 0.f);   // Back-left
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfBase, 0, halfDepth);
    v.normal = glm::vec3(0, -1, 0);
    v.texCoord = glm::vec2(1.f, 1.f);   // Front-right
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfBase, 0, -halfDepth);
    v.normal = glm::vec3(0, -1, 0);
    v.texCoord = glm::vec2(1.f, 0.f);   // Back-right
    vertex_buffer_data.push_back(v);

    // Left slant face - indices 10, 11, 12, 13
    glm::vec3 leftNormal = glm::normalize(glm::vec3(-height, base * 0.5f, 0));

    v.pos = glm::vec3(-halfBase, 0, halfDepth);
    v.normal = leftNormal;
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-front
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(0, height, halfDepth);
    v.normal = leftNormal;
    v.texCoord = glm::vec2(1.f, 0.f);   // Top-front
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfBase, 0, -halfDepth);
    v.normal = leftNormal;
    v.texCoord = glm::vec2(0.f, 1.f);   // Bottom-back
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(0, height, -halfDepth);
    v.normal = leftNormal;
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-back
    vertex_buffer_data.push_back(v);

    // Right slant face - indices 14, 15, 16, 17
    glm::vec3 rightNormal = glm::normalize(glm::vec3(height, base * 0.5f, 0));

    v.pos = glm::vec3(halfBase, 0, -halfDepth);
    v.normal = rightNormal;
    v.texCoord = glm::vec2(0.f, 1.f);   // Bottom-back
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(0, height, -halfDepth);
    v.normal = rightNormal;
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-back
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfBase, 0, halfDepth);
    v.normal = rightNormal;
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-front
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(0, height, halfDepth);
    v.normal = rightNormal;
    v.texCoord = glm::vec2(1.f, 0.f);   // Top-front
    vertex_buffer_data.push_back(v);


    // Indices - all with counter-clockwise winding when viewed from outside
    // Front face
    index_buffer_data.push_back(0);
    index_buffer_data.push_back(1);
    index_buffer_data.push_back(2);

    // Back face
    index_buffer_data.push_back(3);
    index_buffer_data.push_back(4);
    index_buffer_data.push_back(5);

    // Bottom face (2 triangles)
    index_buffer_data.push_back(6);
    index_buffer_data.push_back(7);
    index_buffer_data.push_back(8);
    index_buffer_data.push_back(7);
    index_buffer_data.push_back(9);
    index_buffer_data.push_back(8);

    // Left slant face (2 triangles)
    index_buffer_data.push_back(10);
    index_buffer_data.push_back(11);
    index_buffer_data.push_back(12);
    index_buffer_data.push_back(11);
    index_buffer_data.push_back(13);
    index_buffer_data.push_back(12);

    // Right slant face (2 triangles)
    index_buffer_data.push_back(14);
    index_buffer_data.push_back(15);
    index_buffer_data.push_back(16);
    index_buffer_data.push_back(15);
    index_buffer_data.push_back(17);
    index_buffer_data.push_back(16);

    Mesh* mesh = new Mesh(meshName);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
        &vertex_buffer_data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
        &index_buffer_data[0], GL_STATIC_DRAW);

    mesh->indexSize = index_buffer_data.size();
    mesh->mode = Mesh::DRAW_TRIANGLES;

    return mesh;
}
/******************************************************************************/
Mesh* MeshBuilder::GenerateRectangularPrism(const std::string& meshName, glm::vec3 color,
    float width, float height, float depth)
{
    Vertex v;
    v.color = color;
    std::vector<Vertex> vertex_buffer_data;
    std::vector<GLuint> index_buffer_data;

    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    float halfDepth = depth * 0.5f;

    // Front face (+Z) - 2 triangles
    v.normal = glm::vec3(0, 0, 1);
    v.pos = glm::vec3(halfWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);  // Top-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);  // Bottom-right corner
    vertex_buffer_data.push_back(v);

    // Right face (+X) - 2 triangles
    v.normal = glm::vec3(1, 0, 0);
    v.pos = glm::vec3(halfWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);  // Top-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);  // Bottom-right corner
    vertex_buffer_data.push_back(v);

    // Top face (+Y) - 2 triangles
    v.normal = glm::vec3(0, 1, 0);
    v.pos = glm::vec3(halfWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);  // Top-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);  // Bottom-right corner
    vertex_buffer_data.push_back(v);

    // Back face (-Z) - 2 triangles
    v.normal = glm::vec3(0, 0, -1);
    v.pos = glm::vec3(-halfWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);  // Top-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);  // Bottom-right corner
    vertex_buffer_data.push_back(v);

    // Left face (-X) - 2 triangles
    v.normal = glm::vec3(-1, 0, 0);
    v.pos = glm::vec3(-halfWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);  // Top-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);  // Bottom-right corner
    vertex_buffer_data.push_back(v);

    // Bottom face (-Y) - 2 triangles
    v.normal = glm::vec3(0, -1, 0);
    v.pos = glm::vec3(-halfWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);  // Top-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);  // Top-right corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(halfWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);  // Bottom-left corner (same as above)
    vertex_buffer_data.push_back(v);

    v.pos = glm::vec3(-halfWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);  // Bottom-right corner
    vertex_buffer_data.push_back(v);

    // Indices (simple sequential)
    for (unsigned i = 0; i < 36; ++i)
    {
        index_buffer_data.push_back(i);
    }

    Mesh* mesh = new Mesh(meshName);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
        &vertex_buffer_data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
        &index_buffer_data[0], GL_STATIC_DRAW);

    mesh->indexSize = index_buffer_data.size();
    mesh->mode = Mesh::DRAW_TRIANGLES;

    return mesh;
}


/******************************************************************************/
Mesh* MeshBuilder::GenerateHemisphere(const std::string& meshName, glm::vec3 color,
	unsigned numStack, unsigned numSlice, float radius)
{
	Vertex v;
	v.color = color;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	// Convert to radians
	const float degreePerStack = glm::half_pi<float>() / numStack;  // 90 degrees = π/2
	const float degreePerSlice = glm::two_pi<float>() / numSlice;   // 360 degrees = 2π

	// Bottom center vertex
	v.pos = glm::vec3(0, 0, 0);
	v.normal = glm::vec3(0, -1, 0);
	vertex_buffer_data.push_back(v);

	// Bottom circle vertices
	for (unsigned slice = 0; slice < numSlice + 1; ++slice)
	{
		float theta = slice * degreePerSlice;
		v.pos = glm::vec3(radius * glm::cos(theta), 0, radius * glm::sin(theta));
		v.normal = glm::vec3(0, -1, 0);
        v.texCoord = glm::vec2(0.5f + 0.5f * glm::cos(theta), 0.5f + 0.5f * glm::sin(theta));
		vertex_buffer_data.push_back(v);
	}

	// Bottom circle indices
	for (unsigned slice = 0; slice < numSlice + 1; ++slice)
	{
		index_buffer_data.push_back(0);
		index_buffer_data.push_back(slice + 1);
	}

	// Hemisphere vertices
	unsigned hemisphereStart = vertex_buffer_data.size();
	for (unsigned stack = 0; stack < numStack + 1; ++stack)
	{
		float phi = stack * degreePerStack;  // 0 to π/2
		for (unsigned slice = 0; slice < numSlice + 1; ++slice)
		{
			float theta = slice * degreePerSlice;
			v.pos = glm::vec3(radius * glm::cos(phi) * glm::cos(theta),
				radius * glm::sin(phi),
				radius * glm::cos(phi) * glm::sin(theta));
			v.normal = glm::vec3(glm::cos(phi) * glm::cos(theta),
				glm::sin(phi),
				glm::cos(phi) * glm::sin(theta));
            v.texCoord = glm::vec2((float)slice / numSlice, (float)stack / numStack);
			vertex_buffer_data.push_back(v);
		}
	}

	// Hemisphere indices
	for (unsigned stack = 0; stack < numStack; ++stack)
	{
		for (unsigned slice = 0; slice < numSlice + 1; ++slice)
		{
			index_buffer_data.push_back(hemisphereStart + stack * (numSlice + 1) + slice);
			index_buffer_data.push_back(hemisphereStart + (stack + 1) * (numSlice + 1) + slice);
		}
	}

	Mesh* mesh = new Mesh(meshName);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
		&vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
		&index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}

/******************************************************************************/
Mesh* MeshBuilder::GenerateCube(const std::string& meshName, glm::vec3 color, float length)
{
    Vertex v;
    v.color = color;
    std::vector<Vertex> vertex_buffer_data;
    std::vector<GLuint> index_buffer_data;

    // Front face (+Z)
    v.normal = glm::vec3(0, 0, 1);
    v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(1.f, 0.f);
    vertex_buffer_data.push_back(v);

    // Right face (+X)
    v.normal = glm::vec3(1, 0, 0);
    v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, -0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 0.f);
    vertex_buffer_data.push_back(v);

    // Top face (+Y)
    v.normal = glm::vec3(0, 1, 0);
    v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, 0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(0.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(1.f, 0.f);
    vertex_buffer_data.push_back(v);

    // Back face (-Z)
    v.normal = glm::vec3(0, 0, -1);
    v.pos = glm::vec3(-0.5f * length, 0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(0.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, -0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, 0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, -0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 0.f);
    vertex_buffer_data.push_back(v);

    // Left face (-X)
    v.normal = glm::vec3(-1, 0, 0);
    v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, 0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(0.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(1.f, 0.f);
    vertex_buffer_data.push_back(v);

    // Bottom face (-Y)
    v.normal = glm::vec3(0, -1, 0);
    v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, -0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(0.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.5f * length);
    v.texCoord = glm::vec2(1.f, 0.f);
    vertex_buffer_data.push_back(v);

    // Indices
    for (unsigned i = 0; i < 36; ++i)
    {
        index_buffer_data.push_back(i);
    }

    Mesh* mesh = new Mesh(meshName);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
        &vertex_buffer_data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
        &index_buffer_data[0], GL_STATIC_DRAW);

    mesh->indexSize = index_buffer_data.size();
    mesh->mode = Mesh::DRAW_TRIANGLES;

    return mesh;
}

/******************************************************************************/
Mesh* MeshBuilder::GenerateCylinder(const std::string& meshName, glm::vec3 color,
    unsigned numSlice, float radius, float height)
{
    Vertex v;
    v.color = color;
    std::vector<Vertex> vertex_buffer_data;
    std::vector<GLuint> index_buffer_data;

    float degreePerSlice = glm::two_pi<float>() / numSlice;  // Use radians!

    // Bottom center
    v.pos = glm::vec3(0, -0.5f * height, 0);
    v.normal = glm::vec3(0, -1, 0);
    v.texCoord = glm::vec2(0.5f, 0.5f);  // Center of bottom cap
    vertex_buffer_data.push_back(v);

    // Bottom circle
    for (unsigned slice = 0; slice < numSlice + 1; ++slice)
    {
        float theta = slice * degreePerSlice;
        v.pos = glm::vec3(radius * glm::cos(theta), -0.5f * height, radius * glm::sin(theta));
        v.texCoord = glm::vec2(0.5f + 0.5f * glm::cos(theta), 0.5f + 0.5f * glm::sin(theta));
        v.normal = glm::vec3(0, -1, 0);
        vertex_buffer_data.push_back(v);
    }

    // Bottom indices (triangle fan)
    for (unsigned slice = 0; slice < numSlice + 1; ++slice)
    {
        index_buffer_data.push_back(0);
        index_buffer_data.push_back(slice + 1);
    }

    // Middle (side surface)
    unsigned middleStartIndex = vertex_buffer_data.size();
    for (unsigned slice = 0; slice <= numSlice; ++slice)
    {
        float theta = slice * degreePerSlice;
        float u = (float)slice / numSlice;

        // Bottom vertex
        v.pos = glm::vec3(radius * glm::cos(theta), -height * 0.5f, radius * glm::sin(theta));
        v.normal = glm::vec3(glm::cos(theta), 0, glm::sin(theta));
        v.texCoord = glm::vec2(u, 0.f);  // Bottom of side
        vertex_buffer_data.push_back(v);

        // Top vertex
        v.pos = glm::vec3(radius * glm::cos(theta), height * 0.5f, radius * glm::sin(theta));
        v.normal = glm::vec3(glm::cos(theta), 0, glm::sin(theta));
        v.texCoord = glm::vec2(u, 1.f);  // Top of side
        vertex_buffer_data.push_back(v);
    }

    // Middle indices (triangle strip)
    for (unsigned slice = 0; slice < numSlice + 1; slice++)
    {
        index_buffer_data.push_back(middleStartIndex + 2 * slice + 0);
        index_buffer_data.push_back(middleStartIndex + 2 * slice + 1);
    }

    // Top center
    unsigned topStartIndex = vertex_buffer_data.size();
    v.pos = glm::vec3(0, 0.5f * height, 0);
    v.normal = glm::vec3(0, 1, 0);
    v.texCoord = glm::vec2(0.5f, 0.5f);  // Center of top cap
    vertex_buffer_data.push_back(v);

    // Top circle
    for (unsigned slice = 0; slice < numSlice + 1; ++slice)
    {
        float theta = slice * degreePerSlice;
        v.pos = glm::vec3(radius * glm::cos(theta), 0.5f * height, radius * glm::sin(theta));
        v.normal = glm::vec3(0, 1, 0);
        v.texCoord = glm::vec2(0.5f + 0.5f * glm::cos(theta), 0.5f + 0.5f * glm::sin(theta));
        vertex_buffer_data.push_back(v);
    }

    // Top indices (triangle fan, reversed winding)
    for (unsigned slice = 0; slice < numSlice + 1; ++slice)
    {
        index_buffer_data.push_back(topStartIndex + slice + 1);
        index_buffer_data.push_back(topStartIndex);
    }

    Mesh* mesh = new Mesh(meshName);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
        &vertex_buffer_data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
        &index_buffer_data[0], GL_STATIC_DRAW);

    mesh->indexSize = index_buffer_data.size();
    mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

    return mesh;
}

/******************************************************************************/
Mesh* MeshBuilder::GenerateTrapezoidalPrism(const std::string& meshName, glm::vec3 color, float bottomWidth, float topWidth, float height, float depth)
{
    Vertex v;
    v.color = color;
    std::vector<Vertex> vertex_buffer_data;
    std::vector<GLuint> index_buffer_data;

    // Calculate half dimensions for centering
    float halfBottomWidth = bottomWidth * 0.5f;
    float halfTopWidth = topWidth * 0.5f;
    float halfHeight = height * 0.5f;
    float halfDepth = depth * 0.5f;

    // Front face (+Z) - Trapezoid
    v.normal = glm::vec3(0, 0, 1);
    v.pos = glm::vec3(halfTopWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-right
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfTopWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);   // Top-left
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfBottomWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-left
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfTopWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-right (repeat)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfBottomWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-left (repeat)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfBottomWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);   // Bottom-right
    vertex_buffer_data.push_back(v);

    // Back face (-Z) - Trapezoid
    v.normal = glm::vec3(0, 0, -1);
    v.pos = glm::vec3(-halfTopWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-right (mirrored)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfTopWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);   // Top-left (mirrored)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfBottomWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-left (mirrored)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfTopWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-right (mirrored, repeat)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfBottomWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-left (mirrored, repeat)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfBottomWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);   // Bottom-right (mirrored)
    vertex_buffer_data.push_back(v);

    // Top face (+Y) - Rectangle (smaller)
    v.normal = glm::vec3(0, 1, 0);
    v.pos = glm::vec3(halfTopWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfTopWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfTopWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfTopWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfTopWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfTopWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);
    vertex_buffer_data.push_back(v);

    // Bottom face (-Y) - Rectangle (larger)
    v.normal = glm::vec3(0, -1, 0);
    v.pos = glm::vec3(-halfBottomWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfBottomWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfBottomWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfBottomWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfBottomWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfBottomWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);
    vertex_buffer_data.push_back(v);

    // Right slanted face
    glm::vec3 rightNormal = glm::normalize(glm::cross(
        glm::vec3(0, 0, depth),
        glm::vec3(halfBottomWidth - halfTopWidth, -height, 0)
    ));
    v.normal = rightNormal;
    v.pos = glm::vec3(halfTopWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-back
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfTopWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);   // Top-front
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfBottomWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-front
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfTopWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-back (repeat)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfBottomWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-front (repeat)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(halfBottomWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);   // Bottom-back
    vertex_buffer_data.push_back(v);

    // Left slanted face
    glm::vec3 leftNormal = glm::normalize(glm::cross(
        glm::vec3(halfTopWidth - halfBottomWidth, -height, 0),
        glm::vec3(0, 0, depth)
    ));
    v.normal = leftNormal;
    v.pos = glm::vec3(-halfTopWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-front
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfTopWidth, halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 1.f);   // Top-back
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfBottomWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-back
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfTopWidth, halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 1.f);   // Top-front (repeat)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfBottomWidth, -halfHeight, -halfDepth);
    v.texCoord = glm::vec2(0.f, 0.f);   // Bottom-back (repeat)
    vertex_buffer_data.push_back(v);
    v.pos = glm::vec3(-halfBottomWidth, -halfHeight, halfDepth);
    v.texCoord = glm::vec2(1.f, 0.f);   // Bottom-front
    vertex_buffer_data.push_back(v);

    // Indices
    for (unsigned i = 0; i < 36; ++i)
    {
        index_buffer_data.push_back(i);
    }

    Mesh* mesh = new Mesh(meshName);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
        &vertex_buffer_data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
        &index_buffer_data[0], GL_STATIC_DRAW);

    mesh->indexSize = index_buffer_data.size();
    mesh->mode = Mesh::DRAW_TRIANGLES;

    return mesh;
}

/******************************************************************************/
Mesh* MeshBuilder::GenerateHalfHemisphere(const std::string& meshName, glm::vec3 color,
    unsigned numStack, unsigned numSlice, float radius)
{
    Vertex v;
    v.color = color;
    std::vector<Vertex> vertex_buffer_data;
    std::vector<GLuint> index_buffer_data;

    // Convert to radians
    const float degreePerStack = glm::half_pi<float>() / numStack;  // 90 degrees = π/2
    const float degreePerSlice = glm::pi<float>() / numSlice;       // 180 degrees = π (HALF)

    // Bottom center vertex
    v.pos = glm::vec3(0, 0, 0);
    v.normal = glm::vec3(0, -1, 0);
    v.texCoord = glm::vec2(0.5f, 0.5f);  // Center of bottom half-cap
    vertex_buffer_data.push_back(v);

    // Bottom half-circle vertices
    for (unsigned slice = 0; slice < numSlice + 1; ++slice)
    {
        float theta = slice * degreePerSlice;
        v.pos = glm::vec3(radius * glm::cos(theta), 0, radius * glm::sin(theta));
        v.normal = glm::vec3(0, -1, 0);
        v.texCoord = glm::vec2(0.5f + 0.5f * glm::cos(theta), 0.5f + 0.5f * glm::sin(theta));
        vertex_buffer_data.push_back(v);
    }

    // Bottom half-circle indices
    for (unsigned slice = 0; slice < numSlice + 1; ++slice)
    {
        index_buffer_data.push_back(0);
        index_buffer_data.push_back(slice + 1);
    }

    // Add degenerate triangles to connect bottom to hemisphere
    index_buffer_data.push_back(numSlice + 1);
    index_buffer_data.push_back(numSlice + 2);

    // Hemisphere vertices
    unsigned hemisphereStart = vertex_buffer_data.size();
    for (unsigned stack = 0; stack < numStack + 1; ++stack)
    {
        float phi = stack * degreePerStack;  // 0 to π/2
        for (unsigned slice = 0; slice < numSlice + 1; ++slice)
        {
            float theta = slice * degreePerSlice;  // 0 to π (180 degrees)
            v.pos = glm::vec3(radius * glm::cos(phi) * glm::cos(theta),
                radius * glm::sin(phi),
                radius * glm::cos(phi) * glm::sin(theta));
            v.normal = glm::vec3(glm::cos(phi) * glm::cos(theta),
                glm::sin(phi),
                glm::cos(phi) * glm::sin(theta));
            v.texCoord = glm::vec2((float)slice / numSlice, (float)stack / numStack);
            vertex_buffer_data.push_back(v);
        }
    }

    // Hemisphere indices
    for (unsigned stack = 0; stack < numStack; ++stack)
    {
        // Add degenerate triangle at the start of each new stack (except first)
        if (stack > 0)
        {
            index_buffer_data.push_back(hemisphereStart + stack * (numSlice + 1));
        }

        for (unsigned slice = 0; slice < numSlice + 1; ++slice)
        {
            index_buffer_data.push_back(hemisphereStart + stack * (numSlice + 1) + slice);
            index_buffer_data.push_back(hemisphereStart + (stack + 1) * (numSlice + 1) + slice);
        }

        // Add degenerate triangle at the end of stack (except last)
        if (stack < numStack - 1)
        {
            index_buffer_data.push_back(hemisphereStart + (stack + 1) * (numSlice + 1) + numSlice);
        }
    }

    // Add degenerate triangles to connect hemisphere to back flat face
    index_buffer_data.push_back(hemisphereStart + numStack * (numSlice + 1) + numSlice);
    index_buffer_data.push_back(hemisphereStart + numStack * (numSlice + 1));

    // Back flat face (at theta = 0, on XY plane)
    // Create a strip from bottom to top along the edge
    unsigned backStart = vertex_buffer_data.size();
    for (unsigned stack = 0; stack <= numStack; ++stack)
    {
        float phi = stack * degreePerStack;
        float u = (float)stack / numStack;

        // Vertex with normal pointing back (-z direction)
        v.normal = glm::vec3(0, 0, -1);
        v.pos = glm::vec3(radius * glm::cos(phi), radius * glm::sin(phi), 0);
        v.texCoord = glm::vec2(1.f, u);   // Right edge of flat face
        vertex_buffer_data.push_back(v);

        // Center vertex with same normal
        v.pos = glm::vec3(0, 0, 0);
        v.texCoord = glm::vec2(0.5f, u);  // Center line
        vertex_buffer_data.push_back(v);
    }

    // Back face indices - create strip
    for (unsigned stack = 0; stack <= numStack; ++stack)
    {
        index_buffer_data.push_back(backStart + stack * 2);
        index_buffer_data.push_back(backStart + stack * 2 + 1);
    }

    // Add degenerate triangles to connect back face to front face
    index_buffer_data.push_back(backStart + numStack * 2 + 1);

    // Front flat face (at theta = π, on XY plane)
    // Create a strip from bottom to top along the edge
    unsigned frontStart = vertex_buffer_data.size();
    for (unsigned stack = 0; stack <= numStack; ++stack)
    {
        float phi = stack * degreePerStack;
        float u = (float)stack / numStack;

        // Center vertex with normal pointing forward (+z direction)
        v.normal = glm::vec3(0, 0, -1);
        v.pos = glm::vec3(0, 0, 0);
        v.texCoord = glm::vec2(0.5f, u);  // Center line
        vertex_buffer_data.push_back(v);

        // Edge vertex with same normal
        v.pos = glm::vec3(-radius * glm::cos(phi), radius * glm::sin(phi), 0);
        v.texCoord = glm::vec2(0.f, u);   // Left edge of flat face
        vertex_buffer_data.push_back(v);
    }

    // Add degenerate to connect
    index_buffer_data.push_back(frontStart);

    // Front face indices - create strip
    for (unsigned stack = 0; stack <= numStack; ++stack)
    {
        index_buffer_data.push_back(frontStart + stack * 2);
        index_buffer_data.push_back(frontStart + stack * 2 + 1);
    }

    Mesh* mesh = new Mesh(meshName);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex),
        &vertex_buffer_data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint),
        &index_buffer_data[0], GL_STATIC_DRAW);
    mesh->indexSize = index_buffer_data.size();
    mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;
    return mesh;
}





/******************************************************************************/
Mesh* MeshBuilder::GenerateOBJ(const std::string& meshName, const
    std::string& file_path)
{
    // Read vertices, texcoords & normals from OBJ
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool success = LoadOBJ(file_path.c_str(), vertices, uvs, normals);

    if (!success) { return NULL; }

    // Index the vertices, texcoords & normals properly
    std::vector<Vertex> vertex_buffer_data;
    std::vector<GLuint> index_buffer_data;
    IndexVBO(vertices, uvs, normals, index_buffer_data,
        vertex_buffer_data);


    Mesh* mesh = new Mesh(meshName);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() *
        sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() *
        sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
    mesh->indexSize = index_buffer_data.size();
    mesh->mode = Mesh::DRAW_TRIANGLES;
    return mesh;
}
/******************************************************************************/

Mesh* MeshBuilder::GenerateOBJMTL(const std::string& meshName, const std::string& file_path, const std::string& mtl_path)
{
    //Read vertices, texcoords & normals from OBJ
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<Material> materials;
    bool success = LoadOBJMTL(file_path.c_str(), mtl_path.c_str(), vertices, uvs, normals, materials);
    if (!success) return NULL;
    //Index the vertices, texcoords & normals properly
    std::vector<Vertex> vertex_buffer_data;
    std::vector<GLuint> index_buffer_data;
    IndexVBO(vertices, uvs, normals, index_buffer_data, vertex_buffer_data);
    Mesh* mesh = new Mesh(meshName);
    for (Material& material : materials)
        mesh->materials.push_back(material);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0],
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0],
        GL_STATIC_DRAW);
    mesh->indexSize = index_buffer_data.size();
    mesh->mode = Mesh::DRAW_TRIANGLES;
    return mesh;
}

Mesh* MeshBuilder::GenerateText(const std::string& meshName,
    unsigned numRow, unsigned numCol)
{
    Vertex v;
    std::vector<Vertex> vertex_buffer_data;
    std::vector<unsigned> index_buffer_data;
    float width = 1.f / numCol;
    float height = 1.f / numRow;
    unsigned offset = 0;
    for (unsigned row = 0; row < numRow; ++row)
    {
        for (unsigned col = 0; col < numCol; ++col)
        {
            v.pos = glm::vec3(0.5f, 0.5f, 0.f);
            v.normal = glm::vec3(0, 0, 1);
            v.texCoord = glm::vec2(width * (col + 1), height * (numRow - row));
            vertex_buffer_data.push_back(v);
           
            v.pos = glm::vec3(-0.5f, 0.5f, 0.f);
            v.normal = glm::vec3(0, 0, 1);
            v.texCoord = glm::vec2(width * (col + 0), height * (numRow - row));
            vertex_buffer_data.push_back(v);
            
            v.pos = glm::vec3(-0.5f, -0.5f, 0.f);
            v.normal = glm::vec3(0, 0, 1);
            v.texCoord = glm::vec2(width * (col + 0), height * (numRow - 1 - row));
            vertex_buffer_data.push_back(v);
           
            v.pos = glm::vec3(0.5f, -0.5f, 0.f);
            v.normal = glm::vec3(0, 0, 1);
            v.texCoord = glm::vec2(width * (col + 1), height * (numRow - 1 - row));
            vertex_buffer_data.push_back(v);

            index_buffer_data.push_back(0 + offset);
            index_buffer_data.push_back(1 + offset);
            index_buffer_data.push_back(2 + offset);
            index_buffer_data.push_back(0 + offset);
            index_buffer_data.push_back(2 + offset);
            index_buffer_data.push_back(3 + offset);
            
            offset += 4;

        }
    }
    Mesh* mesh = new Mesh(meshName);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() *
        sizeof(Vertex), &vertex_buffer_data[0],
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        index_buffer_data.size() * sizeof(GLuint),
        &index_buffer_data[0],
        GL_STATIC_DRAW);
    mesh->mode = Mesh::DRAW_TRIANGLES;
    mesh->indexSize = index_buffer_data.size();
    return mesh;
}
