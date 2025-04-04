#pragma once

#include <QOpenGLFunctions>
#include <QVector3D>
#include <QMatrix3x3>
#include <cmath>
#include <cfloat>

#include "Constants.h"

// Sphere Collider
struct SphereCollider 
{
	QVector3D position;
	float radius;

	inline SphereCollider() : radius(1.0f) { }
	inline SphereCollider(const QVector3D& p, float r) :
		position(p), radius(r) { }
};

// Axis-Aligned Bounding Box
struct AABB 
{
	QVector3D position;
	QVector3D size; // HALF SIZE!

	inline AABB() : size(1, 1, 1) { }
	inline AABB(const QVector3D& p, const QVector3D& s) :
		position(p), size(s) { }
};

// Oriented Bounding Box
struct OBB 
{
	QVector3D position;
	QVector3D size; // HALF SIZE!
	QMatrix3x3 orientation; // euler angles

	inline OBB() : size(1, 1, 1) { }
	inline OBB(const QVector3D& p, const QVector3D& s) :
		position(p), size(s) { }
	inline OBB(const QVector3D& p, const QVector3D& s, const QMatrix3x3& o) :
		position(p), size(s), orientation(o) { }
	inline OBB(const AABB& aabb, const QMatrix3x3& o) :
		position(aabb.position), size(aabb.size), orientation(o) { }
};

// Bounding Volume Hierarchy
struct BVHNode 
{
	AABB bounds;
	BVHNode* children;
	int numTriangles;
	int* triangles;

	BVHNode() : children(nullptr), numTriangles(0), triangles(nullptr) {}
};

// Collision Manifold jsp
struct CollisionManifold
{
	bool colliding;
	QVector3D normal;
	float depth;
	std::vector<QVector3D> contacts;
};

void ResetCollisionManifold(CollisionManifold* result);

// Interval jsp
typedef struct Interval {
	float min;
	float max;
} Interval;

QVector3D GetMin(const AABB& aabb); //jsp
QVector3D GetMax(const AABB& aabb); //jsp

AABB FromMinMax(const QVector3D& min, const QVector3D& max); //jsp
OBB AABBToOBB(const AABB& aabb); // util


// Rendering
void Render(const AABB& aabb);
void Render(const OBB& obb);
void Render(const SphereCollider& sphere);
// void Render(const BVHNode& bvh);