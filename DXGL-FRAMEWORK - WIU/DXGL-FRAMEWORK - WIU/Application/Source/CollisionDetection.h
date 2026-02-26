#pragma once

#include "Vector3.h"
#include "PhysicsObject.h"
// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>

struct CollisionData
{
	float penetration{};
	Vector3 collisionNormal; //collision normal is relative to pObj1
	Vector3 contactPoint;
	PhysicsObject* pObj1{};
	PhysicsObject* pObj2{};
};

struct CollisionData3D {
	PhysicsObject* a = nullptr;
	PhysicsObject* b = nullptr;
	Vector3 normal;       // from b toward a
	float penetration = 0.f;
};


//global collision detection functions
bool OverlapCircle2Circle(const Vector3& pos1, float r1, const Vector3& pos2, float r2);
bool OverlapCircle2Circle(PhysicsObject& circle1, float r1, PhysicsObject& circle2, float r2, CollisionData& cd);
bool OverlapAABB2AABB(const Vector3& min1, const Vector3& max1,
					  const Vector3& min2, const Vector3& max2);
bool OverlapAABB2AABB(PhysicsObject& box1, float w1, float h1,
					  PhysicsObject& box2, float w2, float h2, CollisionData& cd);
bool OverlapCircle2Line(const Vector3& circlePos, float radius,
						const Vector3& lineStart, 
						const Vector3& lineEnd);
bool OverlapCircle2OBB(PhysicsObject& circle, float radius, PhysicsObject& box, float w, float h, CollisionData& cd);

//collision resolution function
void ResolveCollision(CollisionData& cd);
void ResolveCircle2StaticLine(PhysicsObject& ball, float radius, const Vector3& lineStart, const Vector3& lineEnd);
bool OverlapCircle2AABB(Vector3 circlePos, float radius, Vector3 boxMin, Vector3 boxMax);
void ResolveCircle2StaticCircle(PhysicsObject& ball1, float radius1, PhysicsObject& ball2, float radius2);


bool OverlapSphere2Sphere(PhysicsObject& a, float ra, PhysicsObject& b, float rb, CollisionData3D& cd);
bool OverlapSphere2AABB(PhysicsObject& sphere, float radius, PhysicsObject& box, glm::vec3 halfExtents,	CollisionData3D& cd); 

void ResolveCollision3D(CollisionData3D& cd);