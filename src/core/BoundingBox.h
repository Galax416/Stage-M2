#pragma once

#include "Utils.h"
#include <QVector3D>
#include <QMatrix3x3>

#include <cmath>


// Forward declaration
struct AABB;
struct OBB; 
QVector3D GetMin(const AABB& aabb);
QVector3D GetMax(const AABB& aabb);
AABB FromMinMax(const QVector3D& min, const QVector3D& max);


// Axis-Aligned Bounding Box
struct AABB 
{
	QVector3D position;
	QVector3D size; // HALF SIZE!

	inline AABB() : size(1, 1, 1) { }
	inline AABB(const QVector3D& p, const QVector3D& s) : position(p), size(s) { }

	inline OBB toOBB() const; // Convert AABB to OBB
	inline bool Intersects(const AABB& other) const;
	float SurfaceArea() const { return 2.0f * (size.x() * size.y() + size.x() * size.z() + size.y() * size.z()); } // Surface area of AABB
};

// Oriented Bounding Box
struct OBB 
{
	QVector3D position;
	QVector3D size;
	QMatrix3x3 orientation;

	inline OBB() : size(1, 1, 1) { }
	inline OBB(const QVector3D& p, const QVector3D& s) : position(p), size(s) { }
	inline OBB(const QVector3D& p, const QVector3D& s, const QMatrix3x3& o) : position(p), size(s), orientation(o) { }
	inline OBB(const AABB& aabb, const QMatrix3x3& o) : position(aabb.position), size(aabb.size), orientation(o) { }

	inline AABB toAABB() const; // Convert OBB to AABB
};

// Convert AABB to OBB
inline OBB AABB::toOBB() const { return OBB(position, size); }

// Check if AABB intersects with another AABB
inline bool AABB::Intersects(const AABB& other) const 
{
    QVector3D minA = GetMin(*this);
    QVector3D maxA = GetMax(*this);
    QVector3D minB = GetMin(other);
    QVector3D maxB = GetMax(other);

	return (minA <= maxB) && (maxA >= minB); // Check if AABB intersects with another AABB
}

// Convert OBB to AABB
inline AABB OBB::toAABB() const
{
	QVector3D axisX(orientation(0, 0), orientation(1, 0), orientation(2, 0));
    QVector3D axisY(orientation(0, 1), orientation(1, 1), orientation(2, 1));
    QVector3D axisZ(orientation(0, 2), orientation(1, 2), orientation(2, 2));

    QVector3D absX = axisX * std::abs(size.x());
    QVector3D absY = axisY * std::abs(size.y());
    QVector3D absZ = axisZ * std::abs(size.z());

    QVector3D halfExtents = QVector3D(
        std::abs(absX.x()) + std::abs(absY.x()) + std::abs(absZ.x()),
        std::abs(absX.y()) + std::abs(absY.y()) + std::abs(absZ.y()),
        std::abs(absX.z()) + std::abs(absY.z()) + std::abs(absZ.z())
    );

    return AABB(position, halfExtents);
}


// Functions to get min and max of AABB
inline QVector3D GetMin(const AABB& aabb)
{
	QVector3D p1 = aabb.position + aabb.size;
	QVector3D p2 = aabb.position - aabb.size;

	return QVector3D(fminf(p1.x(), p2.x()), fminf(p1.y(), p2.y()), fminf(p1.z(), p2.z()));
}

inline QVector3D GetMax(const AABB& aabb)
{
	QVector3D p1 = aabb.position + aabb.size;
	QVector3D p2 = aabb.position - aabb.size;

	return QVector3D(fmaxf(p1.x(), p2.x()), fmaxf(p1.y(), p2.y()), fmaxf(p1.z(), p2.z()));
}

// Convert min and max to AABB
inline AABB FromMinMax(const QVector3D& min, const QVector3D& max)
{
	return AABB((min + max) * 0.5f, (max - min) * 0.5f);
}