#pragma once

#include "Utils.h"
#include <QVector3D>
#include <QMatrix3x3>
#include <QMatrix4x4>

#include <cmath>
#include <cfloat>

// Forward declaration
struct AABB;
struct OBB; 
QVector3D GetMin(const AABB& aabb);
QVector3D GetMax(const AABB& aabb);
AABB FromMinMax(const QVector3D& min, const QVector3D& max);


// Axis-Aligned Bounding Box
struct AABB 
{
	QVector3D center;
	QVector3D size; // HALF SIZE!

	inline AABB() : size(1, 1, 1) { }
	inline AABB(const QVector3D& p, const QVector3D& s) : center(p), size(s) { }

	inline OBB toOBB() const; // Convert AABB to OBB
	inline bool Intersects(const AABB& other) const;
	float SurfaceArea() const { return 2.0f * (size.x() * size.y() + size.x() * size.z() + size.y() * size.z()); } // Surface area of AABB
    void Expand(float amount) { size += QVector3D(amount, amount, amount); } // Expand AABB by a given amount
};

// Oriented Bounding Box
struct OBB 
{
	QVector3D center;
	QVector3D size; // HALF SIZE!
	QMatrix3x3 orientation;

	inline OBB() : size(1, 1, 1) { }
	inline OBB(const QVector3D& p, const QVector3D& s) : center(p), size(s) { }
	inline OBB(const QVector3D& p, const QVector3D& s, const QMatrix3x3& o) : center(p), size(s), orientation(o) { }
	inline OBB(const AABB& aabb, const QMatrix3x3& o) : center(aabb.center), size(aabb.size), orientation(o) { }

	inline AABB toAABB() const; // Convert OBB to AABB
	QVector3D GetAxis(int i) const { return QVector3D(orientation(0, i), orientation(1, i), orientation(2, i)); } // Get axis of OBB
	inline QVector3D ClosestPoint(const QVector3D& point) const; // Get closest point on OBB to a given point
};

// Convert AABB to OBB
inline OBB AABB::toOBB() const { return OBB(center, size); }

// Check if AABB intersects with another AABB
inline bool AABB::Intersects(const AABB& other) const 
{
    QVector3D minA = GetMin(*this);
    QVector3D maxA = GetMax(*this);
    QVector3D minB = GetMin(other);
    QVector3D maxB = GetMax(other);

	return (minA.x() <= maxB.x() && minA.y() <= maxB.y() && minA.z() <= maxB.z()) &&
		   (maxA.x() >= minB.x() && maxA.y() >= minB.y() && maxA.z() >= minB.z()); // Check if AABB intersects with another AABB
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

    return AABB(center, halfExtents);
}

// Get closest point on OBB to a given point
inline QVector3D OBB::ClosestPoint(const QVector3D& point) const
{
	QVector3D localPoint = QMatrix4x4(orientation.transposed()).map(point - center);

	QVector3D clampedPoint = localPoint;
	clampedPoint.setX(qBound(-size.x(), clampedPoint.x(), size.x()));
	clampedPoint.setY(qBound(-size.y(), clampedPoint.y(), size.y()));
	clampedPoint.setZ(qBound(-size.z(), clampedPoint.z(), size.z()));

	return center + QMatrix4x4(orientation).map(clampedPoint);
}

// Functions to get min and max of AABB
inline QVector3D GetMin(const AABB& aabb)
{
	QVector3D p1 = aabb.center + aabb.size;
	QVector3D p2 = aabb.center - aabb.size;

	return QVector3D(fminf(p1.x(), p2.x()), fminf(p1.y(), p2.y()), fminf(p1.z(), p2.z()));
}

inline QVector3D GetMax(const AABB& aabb)
{
	QVector3D p1 = aabb.center + aabb.size;
	QVector3D p2 = aabb.center - aabb.size;

	return QVector3D(fmaxf(p1.x(), p2.x()), fmaxf(p1.y(), p2.y()), fmaxf(p1.z(), p2.z()));
}

// Convert min and max to AABB
inline AABB FromMinMax(const QVector3D& min, const QVector3D& max)
{
	return AABB((min + max) * 0.5f, (max - min) * 0.5f);
}

// SAT (Separating Axis Theorem) for OBB collision detection
// Returns true if there is a collision and sets outMTV to the minimum translation vector
inline bool TestOBBOBBCollision(const OBB& a, const OBB& b, QVector3D& outMTV)
{
    const float EPSILON = 1e-6f;
    float minOverlap = FLT_MAX;
    QVector3D smallestAxis;

    QVector3D axes[15];
    int axisCount = 0;

    // Axes from A (local X, Y, Z transformés)
    for (int i = 0; i < 3; ++i)
        axes[axisCount++] = a.GetAxis(i).normalized();

    // Axes from B
    for (int i = 0; i < 3; ++i)
        axes[axisCount++] = b.GetAxis(i).normalized();

    // Cross products of all axes
    for (int i = 0; i < 3; ++i) {
        QVector3D aAxis = a.GetAxis(i);
        for (int j = 0; j < 3; ++j) {
            QVector3D bAxis = b.GetAxis(j);
            QVector3D axis = QVector3D::crossProduct(aAxis, bAxis);
            if (axis.lengthSquared() > EPSILON) {
                axes[axisCount++] = axis.normalized();
            }
        }
    }

    QVector3D delta = b.center - a.center;

    for (int i = 0; i < axisCount; ++i) {
        QVector3D axis = axes[i];
        if (axis.lengthSquared() < EPSILON) continue;

        // Projection of box A onto axis
        float projA = 0.0f;
        for (int j = 0; j < 3; ++j) projA += a.size[j] * fabs(QVector3D::dotProduct(axis, a.GetAxis(j)));

        // Projection of box B onto axis
        float projB = 0.0f;
        for (int j = 0; j < 3; ++j) projB += b.size[j] * fabs(QVector3D::dotProduct(axis, b.GetAxis(j)));

        float distance = fabs(QVector3D::dotProduct(delta, axis));
        float overlap = projA + projB - distance;

        if (overlap < 0.0f) return false; // Separating axis found -> no collision

        // Store the axis with the least overlap
        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;

            // Ensure MTV points from A to B
            if (QVector3D::dotProduct(smallestAxis, delta) < 0.0f) smallestAxis = -smallestAxis;
        }
    }

    // Final MTV
    outMTV = smallestAxis * minOverlap;
    return true;
}

