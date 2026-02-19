#include "MeshBuilder.h"
#include <GL\glew.h>
#include <vector>

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
Mesh* MeshBuilder::GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	//x-axis
	v.pos = glm::vec3(-lengthX, 0, 0);	v.color = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(lengthX, 0, 0);	v.color = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	//y-axis
	v.pos = glm::vec3(0, -lengthY, 0);	v.color = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0, lengthY, 0);	v.color = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	//z-axis
	v.pos = glm::vec3(0, 0, -lengthZ);	v.color = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0, 0, lengthZ);	v.color = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);

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
Mesh* MeshBuilder::GenerateQuad(const std::string &meshName, glm::vec3 color, float length)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	v.color = color;

	// Add the vertices here
	v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.f);	v.normal = glm::vec3(0, 0, 1); v.texCoord = glm::vec2(1, 0); vertex_buffer_data.push_back(v); //v3
	v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.f);	v.normal = glm::vec3(0, 0, 1); v.texCoord = glm::vec2(1, 1); vertex_buffer_data.push_back(v); //v0
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.f);	v.normal = glm::vec3(0, 0, 1); v.texCoord = glm::vec2(0, 1); vertex_buffer_data.push_back(v); //v1
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.f);	v.normal = glm::vec3(0, 0, 1); v.texCoord = glm::vec2(0, 0); vertex_buffer_data.push_back(v); //v2


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

Mesh* MeshBuilder::GenerateSphere(const std::string& meshName, glm::vec3 color, float radius, int numSlice, int numStack)
{
	Vertex v; // Vertex definition
	std::vector<Vertex> vertex_buffer_data; // Vertex Buffer Objects(VBOs)
	std::vector<GLuint> index_buffer_data; // Element Buffer Objects(EBOs)

	v.color = color;

	float degreePerStack = glm::pi<float>() / numStack;
	float degreePerSlice = glm::two_pi<float>() / numSlice;

	// Generate the sphere vertices
	for (unsigned stack = 0; stack < numStack + 1; ++stack) //stack
	{
		float phi = -glm::half_pi<float>() + stack * degreePerStack;
		for (unsigned slice = 0; slice < numSlice + 1; ++slice) //slice
		{
			float theta = slice * degreePerSlice;
			v.pos = glm::vec3(radius * glm::cos(phi) * glm::cos(theta), 
							  radius * glm::sin(phi), 
							  radius * glm::cos(phi) * glm::sin(theta));
			v.normal = glm::vec3(glm::cos(phi) * glm::cos(theta), 
				       glm::sin(phi), 
				       glm::cos(phi) * glm::sin(theta));
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

Mesh* MeshBuilder::GenerateCylinder(const std::string& meshName, glm::vec3 color, unsigned numSlice, float radius, float height)
{
	Vertex v;
	v.color = color;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	float degreePerSlice = 360.0f / numSlice;

	// Bottom
	v.pos = glm::vec3(0, -0.5f * height, 0); v.normal = glm::vec3(0, -1, 0);  vertex_buffer_data.push_back(v);
	
	for (unsigned slice = 0; slice < numSlice + 1; ++slice)
	{
		float theta = glm::radians(slice * degreePerSlice);
		v.pos = glm::vec3(radius * cos(theta), -0.5f * height, radius * sin(theta));
		v.normal = glm::vec3(0, -1, 0);
		vertex_buffer_data.push_back(v);
	}

	for (int slice = 0; slice < numSlice + 1; ++slice)
	{
		index_buffer_data.push_back(0);
		index_buffer_data.push_back(slice + 1);

	}

	unsigned middleStartIndex = vertex_buffer_data.size();
	
	for (int slice = 0; slice <= numSlice; ++slice) { // slice
		float theta = glm::radians(slice * degreePerSlice);
		v.pos = glm::vec3(radius * cos(theta), -height * 0.5f, radius * sin(theta));
		v.normal = glm::vec3(cos(theta), 0, sin(theta));
		vertex_buffer_data.push_back(v);

		v.pos = glm::vec3(radius * cos(theta), height * 0.5f, radius * sin(theta));
		v.normal = glm::vec3(cos(theta), 0, sin(theta));
		vertex_buffer_data.push_back(v);

	}

	for (unsigned slice = 0; slice < numSlice + 1; slice++) {
		index_buffer_data.push_back(middleStartIndex + 2 * slice + 0);
		index_buffer_data.push_back(middleStartIndex + 1 + 2 * slice + 0);
	}

	// top
	unsigned topStartIndex = vertex_buffer_data.size();
	
	v.pos = glm::vec3(0, 0.5f * height, 0); v.normal = glm::vec3(0, 1, 0);  vertex_buffer_data.push_back(v);
	for (unsigned slice = 0; slice < numSlice + 1; ++slice)
	{
		float theta = glm::radians(slice * degreePerSlice);
		v.pos = glm::vec3(radius * cos(theta), 0.5f * height, radius * sin(theta));
		v.normal = glm::vec3(0, 1, 0);
		vertex_buffer_data.push_back(v);
	}

	for (int slice = 0; slice < numSlice + 1; ++slice)
	{
		index_buffer_data.push_back(topStartIndex + slice + 1);
		index_buffer_data.push_back(topStartIndex);
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

Mesh* MeshBuilder::GenerateCube(const std::string& meshName, glm::vec3 color, float length)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	v.color = color;

	//front face (+Z)
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.5f * length);	v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);	v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.5f * length);  	v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.5f * length);	v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.5f * length);	    v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);	v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);

	//right face (+X)
	v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, -0.5f * length, -0.5f * length);	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);  	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.5f * length);	    v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);

	//top face (+Y)
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, 0.5f * length, 0.5f * length);	    v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);

	//back face (-Z)
	v.pos = glm::vec3(0.5f * length, -0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, -0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, 0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);

	//left face (-X)
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.5f * length);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, 0.5f * length);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, 0.5f * length, -0.5f * length);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);

	//bottom face (-Y)
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, -0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, -0.5f * length);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(0.5f * length, -0.5f * length, 0.5f * length);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-0.5f * length, -0.5f * length, 0.5f * length);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);



	for (unsigned i = 0; i < 36; ++i)
	{
		index_buffer_data.push_back(i);
	}

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

Mesh* MeshBuilder::GenerateCuboid(const std::string& meshName, glm::vec3 color, float lengthX, float lengthY, float lengthZ)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	v.color = color;

	// Calculate half dimensions for centering
	// Swap X and Z so short side is on X-axis (red) and long side is on Z-axis (blue)
	float halfX = lengthZ / 2.0f;
	float halfY = lengthY / 2.0f;
	float halfZ = lengthX / 2.0f;

	//front face (+Z)
	v.pos = glm::vec3(-halfX, -halfY, halfZ);	v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, -halfY, halfZ);	v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, halfY, halfZ);		v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, -halfY, halfZ);	v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, halfY, halfZ);		v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, halfY, halfZ);	v.normal = glm::vec3(0, 0, 1);	vertex_buffer_data.push_back(v);

	//right face (+X)
	v.pos = glm::vec3(halfX, -halfY, halfZ);	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, -halfY, -halfZ);	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, halfY, -halfZ);	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, -halfY, halfZ);	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, halfY, -halfZ);	v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, halfY, halfZ);		v.normal = glm::vec3(1, 0, 0);	vertex_buffer_data.push_back(v);

	//top face (+Y)
	v.pos = glm::vec3(-halfX, halfY, halfZ);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, halfY, halfZ);		v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, halfY, -halfZ);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, halfY, halfZ);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, halfY, -halfZ);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, halfY, -halfZ);	v.normal = glm::vec3(0, 1, 0);	vertex_buffer_data.push_back(v);

	//back face (-Z)
	v.pos = glm::vec3(halfX, -halfY, -halfZ);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, -halfY, -halfZ);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, halfY, -halfZ);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, -halfY, -halfZ);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, halfY, -halfZ);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, halfY, -halfZ);	v.normal = glm::vec3(0, 0, -1);	vertex_buffer_data.push_back(v);

	//left face (-X)
	v.pos = glm::vec3(-halfX, -halfY, -halfZ);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, -halfY, halfZ);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, halfY, halfZ);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, -halfY, -halfZ);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, halfY, halfZ);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, halfY, -halfZ);	v.normal = glm::vec3(-1, 0, 0);	vertex_buffer_data.push_back(v);

	//bottom face (-Y)
	v.pos = glm::vec3(-halfX, -halfY, -halfZ);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, -halfY, -halfZ);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, -halfY, halfZ);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, -halfY, -halfZ);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(halfX, -halfY, halfZ);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);
	v.pos = glm::vec3(-halfX, -halfY, halfZ);	v.normal = glm::vec3(0, -1, 0);	vertex_buffer_data.push_back(v);


	for (unsigned i = 0; i < 36; ++i)
	{
		index_buffer_data.push_back(i);
	}

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

Mesh* MeshBuilder::GenerateHemisphere(const std::string& meshName, glm::vec3 color, unsigned numStack, unsigned numSlice, float radius)
{
	Vertex v;
	v.color = color;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	const float degreePerStack = 90.f / numStack;
	const float degreePerSlice = 360.f / numSlice;
	const float radPerStack = glm::radians(degreePerStack);
	const float radPerSlice = glm::radians(degreePerSlice);

	v.pos = glm::vec3(0, 0, 0);
	v.normal = glm::vec3(0, -1, 0);
	vertex_buffer_data.push_back(v);

	for (unsigned slice = 0; slice < numSlice + 1; ++slice) //slice
	{
		float theta = slice * radPerSlice;
		v.pos = glm::vec3(radius * cos(theta), 0, radius * sin(theta));
		v.normal = glm::vec3(0, -1, 0);
		vertex_buffer_data.push_back(v);
	}

	for (unsigned slice = 0; slice < numSlice + 1; ++slice) //slice
	{
		index_buffer_data.push_back(0);
		index_buffer_data.push_back(slice + 1);
	}

	unsigned hemisphereStart = vertex_buffer_data.size();
	for (unsigned stack = 0; stack < numStack + 1; ++stack) //stack //replace with 180 for sphere
	{
		float phi = stack * radPerStack;
		for (unsigned slice = 0; slice < numSlice + 1; ++slice) //slice
		{
			float theta = slice * radPerSlice;
			v.pos = glm::vec3(radius * cosf(phi) * cosf(theta), radius * sinf(phi), radius * cosf(phi) * sinf(theta));
			v.normal = glm::vec3(cosf(phi) * cosf(theta), sinf(phi), cosf(phi) * sinf(theta));
			vertex_buffer_data.push_back(v);
		}
	}

	for (unsigned stack = 0; stack < numStack; ++stack)
	{
		for (unsigned slice = 0; slice < numSlice + 1; ++slice)
		{
			index_buffer_data.push_back(hemisphereStart + (numSlice + 1) * stack + slice + 0);
			index_buffer_data.push_back(hemisphereStart + (numSlice + 1) * (stack + 1) + slice + 0);
		}
	}

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	
	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;
	
	return mesh;
}

Mesh* MeshBuilder::GenerateCone(const std::string& meshName, glm::vec3 color, unsigned numStack, unsigned numSlice, float radius, float height)
{
	Vertex v;
	v.color = color;

	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	const float degreePerSlice = 360.f / numSlice;
	const float radPerSlice = glm::radians(degreePerSlice);

	// Base center
	v.pos = glm::vec3(0, 0, 0);
	v.normal = glm::vec3(0, -1, 0);
	vertex_buffer_data.push_back(v);

	// Base vertices
	for (unsigned slice = 0; slice < numSlice + 1; ++slice)
	{
		float theta = slice * radPerSlice;
		v.pos = glm::vec3(radius * cos(theta), 0, radius * sin(theta));
		v.normal = glm::vec3(0, -1, 0);
		vertex_buffer_data.push_back(v);
	}

	// Side vertices
	for (unsigned slice = 0; slice < numSlice + 1; ++slice)
	{
		float theta = slice * radPerSlice;
		float x = radius * cos(theta);
		float z = radius * sin(theta);
		v.pos = glm::vec3(x, 0, z);
		glm::vec3 sideNormal = glm::normalize(glm::vec3(x, radius / height, z));
		v.normal = sideNormal;
		vertex_buffer_data.push_back(v);
	}

	// Cone tip
	v.pos = glm::vec3(0, height, 0);
	v.normal = glm::vec3(0, 1, 0);
	vertex_buffer_data.push_back(v);

	// ===== REPLACE EVERYTHING FROM HERE =====
	unsigned tipIndex = vertex_buffer_data.size() - 1;
	unsigned sideVertexStart = numSlice + 2;

	// BASE as TRIANGLE STRIP (fan pattern)
	for (unsigned slice = 1; slice <= numSlice + 1; ++slice)
	{
		index_buffer_data.push_back(0);      // center
		index_buffer_data.push_back(slice);  // base vertex
	}

	// Add degenerate triangles to connect base to side
	index_buffer_data.push_back(0);  // degenerate
	index_buffer_data.push_back(sideVertexStart);  // degenerate

	// SIDE TRIANGLE STRIP
	for (unsigned slice = 0; slice <= numSlice; ++slice)
	{
		index_buffer_data.push_back(sideVertexStart + slice); // base
		index_buffer_data.push_back(tipIndex);                 // tip
	}
	// ===== TO HERE =====

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}

Mesh* MeshBuilder::GenerateOBJ(const std::string& meshName, const std::string& file_path)
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
	IndexVBO(vertices, uvs, normals, index_buffer_data, vertex_buffer_data);

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;
	return mesh;
}

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
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;
	return mesh;
}

Mesh* MeshBuilder::GenerateText(const std::string& meshName, unsigned numRow, unsigned numCol)
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
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0],
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0],
		GL_STATIC_DRAW);
	mesh->mode = Mesh::DRAW_TRIANGLES;
	mesh->indexSize = index_buffer_data.size();

	return mesh;
}
