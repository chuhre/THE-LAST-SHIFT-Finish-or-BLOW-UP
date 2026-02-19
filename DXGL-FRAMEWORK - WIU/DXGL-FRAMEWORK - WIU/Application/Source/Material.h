#ifndef MATERIAL_H
#define MATERIAL_H

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>



struct Material
{
	glm::vec3 kAmbient;
	glm::vec3 kDiffuse;
	glm::vec3 kSpecular;
	float kShininess;
	int size; // Add this member to track the number of vertices using this material

	Material() :
		kAmbient(0.0f, 0.0f, 0.0f), kDiffuse(0.0f, 0.0f, 0.0f), kSpecular(0.0f, 0.0f, 0.0f),
		kShininess(1.f),
		size(0)
	{
	}
};


#endif