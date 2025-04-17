#pragma once

#include <QVector3D>

// Sphere Collider
struct SphereCollider 
{
	QVector3D position;
	float radius;

	inline SphereCollider() : radius(1.0f) { }
	inline SphereCollider(const QVector3D& p, float r) : position(p), radius(r) { }
};