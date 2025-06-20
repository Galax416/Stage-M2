#pragma once

#include <QVector3D>

// Sphere Collider
struct SphereCollider 
{
	QVector3D center;
	float radius;

	inline SphereCollider() : radius(1.0f) { }
	inline SphereCollider(const QVector3D& p, float r) : center(p), radius(r) { }
};