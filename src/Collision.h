#pragma once

#include <QVector3D>
#include <QMatrix3x3>

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