#include "CollisionDetection.h"

bool OverlapCircle2Circle(const Vector3& pos1, float r1, const Vector3& pos2, float r2)
{
	float lengthSq = (pos1 - pos2).LengthSquared();
	return lengthSq <= (r1 + r2) * (r1 + r2);
}

bool OverlapCircle2Circle(PhysicsObject& circle1, float r1, PhysicsObject& circle2, float r2, CollisionData& cd)
{
	Vector3 disp = circle1.pos - circle2.pos;
	if (disp.LengthSquared() <= (r1 + r2) * (r1 + r2))
	{
		cd.pObj1 = &circle1;
		cd.pObj2 = &circle2;
		cd.penetration = (r1 + r2) - disp.Length();
		cd.collisionNormal = disp.Normalized();
		return true;
	}

	// no collision detected
	return false;
}

bool OverlapAABB2AABB(const Vector3& min1, const Vector3& max1,
					  const Vector3& min2, const Vector3& max2)
{
	return !(max1.x < min2.x || 
			 max1.y < min2.y ||
			 max2.x < min1.x || 
		     max2.y < min1.y);
}

bool OverlapAABB2AABB(PhysicsObject& box1, float w1, float h1,
					  PhysicsObject& box2, float w2, float h2, CollisionData& cd)
{
	Vector3 extent1 = Vector3{ w1 * 0.5f, h1 * 0.5f };
	Vector3 extent2 = Vector3{ w2 * 0.5f, h2 * 0.5f };

	Vector3 min1 = box1.pos - extent1;	
	Vector3 max1 = box1.pos + extent1;
	Vector3 min2 = box2.pos - extent2;
	Vector3 max2 = box2.pos + extent2;

	if (OverlapAABB2AABB(min1, max1, min2, max2))
	{
		cd.pObj1 = &box1;
		cd.pObj2 = &box2;
		//calculate penetration depth and collision normal
		float penLeft = (max2.x - min1.x);
		float penRight = (max1.x - min2.x);
		float penBottom = (max2.y - min1.y);
		float penTop = (max1.y - min2.y);
		float minPenX = (penLeft < penRight) ? penLeft : -penRight;
		float minPenY = (penBottom < penTop) ? penBottom : -penTop;
		if (fabs(minPenX) < fabs(minPenY))
		{
			cd.penetration = fabs(minPenX);
			cd.collisionNormal = (minPenX > 0) ? Vector3{ 1, 0, 0 } : Vector3{ -1, 0, 0 };
		}
		else
		{
			cd.penetration = fabs(minPenY);
			cd.collisionNormal = (minPenY > 0) ? Vector3{ 0, 1, 0 } : Vector3{ 0, -1, 0 };
		}
		return true;
	}
	return false;
}

bool OverlapCircle2Line(const Vector3& circlePos, float radius,
						const Vector3& lineStart,
						const Vector3& lineEnd)
{
	Vector3 lineTangent = lineEnd - lineStart;
	if (lineTangent.IsZero()) return false;  //this is not a line, but a point. reject collision test

	float lineLength = lineTangent.Length();
	lineTangent.Normalize();

	Vector3 normal = Vector3(-lineTangent.y, lineTangent.x); //CCW 90-deg rotation
	normal.Normalize();

	Vector3 disp = circlePos - lineStart;
	float shortestDist = fabs(disp.Dot(normal));

	//early out
	if (shortestDist > radius) {
		return false;
	}

	//project disp onto line tangent to get distance along line
	float distAlongLine = disp.Dot(lineTangent);
	if (distAlongLine < 0.f || distAlongLine > lineLength) {
		return false;
	}
	return true;
}

bool OverlapCircle2OBB(PhysicsObject& circle, float radius, PhysicsObject& box, float w, float h, CollisionData& cd)
{
	float angleRad = box.angularVel; // or however your wall stores its angle in radians
	float cosA = cosf(angleRad);
	float sinA = sinf(angleRad);

	// Vector from box center to circle
	Vector3 diff = circle.pos - box.pos;

	// Project onto box local axes
	float localX = diff.x * cosA + diff.y * sinA;
	float localY = -diff.x * sinA + diff.y * cosA;

	float halfW = w * 0.5f;
	float halfH = h * 0.5f;

	// Clamp to box extents
	float closestX = localX;
	float closestY = localY;

	if (closestX > halfW)
		closestX = halfW;
	if (closestX < -halfW)
		closestX = -halfW;
	if (closestY > halfH)
		closestY = halfH;
	if (closestY < -halfH)
		closestY = -halfH;

	// Convert closest point back to world space
	Vector3 closestWorld;
	closestWorld.x = box.pos.x + closestX * cosA - closestY * sinA;
	closestWorld.y = box.pos.y + closestX * sinA + closestY * cosA;
	closestWorld.z = 0;

	Vector3 toCircle = circle.pos - closestWorld;
	float dist = toCircle.Length();

	if (dist < radius)
	{
		cd.pObj1 = &circle;
		cd.pObj2 = &box;
		cd.collisionNormal = toCircle.Normalized();
		cd.penetration = radius - dist;
		return true;
	}

	return false;
}

void ResolveCircle2StaticLine(PhysicsObject& ball, float radius, const Vector3& lineStart, const Vector3& lineEnd)
{
	Vector3 lineTangent = lineEnd - lineStart;
	float lineLength = lineTangent.Length();
	lineTangent.Normalize();

	float penetrationDist{};
	Vector3 normal{};
	Vector3 lineVec = ball.pos - lineStart;
	float projectedDist = lineVec.Dot(lineTangent);

	//ball is to the "right" of lineEnd
	if (projectedDist > lineLength)
	{
		//TODO:
		//1) calculate the collision normal
		//2) calculate the penetration distance

		normal = (ball.pos - lineEnd).Normalized();
		penetrationDist = radius - (ball.pos - lineEnd).Length();
	}
	//ball is to the "left" of lineStart
	else if (projectedDist < 0)
	{
		//TODO:
		//1) calculate the collision normal
		//2) calculate the penetration distance

		normal = (ball.pos - lineStart).Normalized();
		penetrationDist = radius - (ball.pos - lineStart).Length();
	}
	//ball is within line segment
	else
	{
		//TODO:
		//1) calculate the collision normal
		//2) calculate the penetration distance

		normal = Vector3(-lineTangent.y, lineTangent.x, 0).Normalized();

		float dist = (ball.pos - lineStart).Dot(normal);  // perpendicular distance
		penetrationDist = radius - dist;
	}

	//resolve penetration and velocity
	ball.pos += normal * penetrationDist;
	ball.vel -= ball.vel.Dot(normal) * normal;

}

bool OverlapCircle2AABB(Vector3 circlePos, float radius, Vector3 boxMin, Vector3 boxMax)
{
	// Find closest point on AABB to circle center
	Vector3 closestPoint;
	closestPoint.x = fmax(boxMin.x, fmin(circlePos.x, boxMax.x));
	closestPoint.y = fmax(boxMin.y, fmin(circlePos.y, boxMax.y));
	closestPoint.z = 0.0f;
	// fmax is used to get the larger of two values
	// fmin is used to get the smaller of two values

	// Calculate distance from circle center to closest point
	Vector3 diff = circlePos - closestPoint;
	float distSq = diff.LengthSquared();

	// Check if distance is less than radius
	return distSq < radius * radius;
}

void ResolveCollision(CollisionData& cd)
{
	PhysicsObject& obj1 = *cd.pObj1;
	PhysicsObject& obj2 = *cd.pObj2;

	float invMass1 = (obj1.mass == 0.f ? 0.f : 1.f / obj1.mass);
	float invMass2 = (obj2.mass == 0.f ? 0.f : 1.f / obj2.mass);
	float totalInvMass = invMass1 + invMass2;

	if (totalInvMass == 0.f) return;

	totalInvMass = 1.f / totalInvMass;

	// resolve penetration
	cd.penetration = cd.penetration * totalInvMass;
	cd.collisionNormal = cd.collisionNormal.Normalized();

	obj1.pos += cd.collisionNormal * cd.penetration * invMass1;
	obj2.pos -= cd.collisionNormal * cd.penetration * invMass2;

	// resolve velocity
	Vector3 relativeVel = obj1.vel - obj2.vel;
	float velAlongNormal = relativeVel.Dot(cd.collisionNormal);

	if (velAlongNormal > 0.f) return; //objects are separating

	// resolve impulse and bounciness
    float bounciness = std::max(obj1.bounciness, obj2.bounciness);
	float impulseMag = -(1.f + bounciness) * velAlongNormal;
	impulseMag *= totalInvMass;
 
	// what this does is calculate the impulse vector by scaling the collision normal
	// so that it has the correct magnitude to change the velocities of the colliding objects appropriately
	// based on their masses and the coefficient of restitution (bounciness).
	Vector3 impulse = impulseMag * cd.collisionNormal;
	obj1.vel += impulse * invMass1;
	obj2.vel -= impulse * invMass2;

	// account for angular velocity at point of collision
	Vector3 vel1 = obj1.vel;
	Vector3 vel2 = obj2.vel;
	if (obj1.angularVel != 0.f)
	{
		float angularVelRad = Math::DegreeToRadian(obj1.angularVel);
		vel1 += angularVelRad * Vector3(-cd.collisionNormal.y, cd.collisionNormal.x, 0.f);
	}
	if (obj2.angularVel != 0.f)
	{
		float angularVelRad = Math::DegreeToRadian(obj2.angularVel);
		vel2 += angularVelRad * Vector3(cd.collisionNormal.y, -cd.collisionNormal.x, 0.f);
	}
}

void ResolveCircle2StaticCircle(PhysicsObject& ball1, float radius1, PhysicsObject& ball2, float radius2)
{
	// calculate collision normal
	Vector3 collisionNormal = ball1.pos - ball2.pos;
	collisionNormal.Normalize();

	// calculate penetration distance
	float penetrationDist = (radius1 + radius2) - (ball1.pos - ball2.pos).Length();

	// separate ball1 from ball2
	ball1.pos += collisionNormal * penetrationDist;

	// reflect ball1's velocity
	ball1.vel -= (1.f + ball1.bounciness) * ball1.vel.Dot(collisionNormal) * collisionNormal;

}