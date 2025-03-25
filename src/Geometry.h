#pragma once

#include <QOpenGLFunctions>
#include <QVector2D>
#include <QVector3D>
#include <QtMath>
#include <QDebug>
#include <qgenericmatrix.h>

#include "Model.h"

#define PI 3.14159265358979323846f


// 2D Geometry
typedef QVector2D Point2D;

struct Circle 
{
	Point2D position;
	float radius;

	inline Circle() : radius(1.0f) {}
	inline Circle(const Point2D& p, float r):
		position(p), radius(r) {}
};

// Renderings 2D
void Render(const Circle& circle);

// 3D Geometry
typedef QVector3D Point;

struct Line 
{
	Point start;
	Point end;

	inline Line() {}
	inline Line(const Point& s, const Point& e) :
		start(s), end(e) { }
};

struct Ray 
{
	Point origin;
	QVector3D direction;

	inline Ray() : direction(0.0f, 0.0f, 1.0f) {}
	inline Ray(const Point& o, const QVector3D& d) :
		origin(o), direction(d) {
		NormalizeDirection();
	}
	inline void NormalizeDirection() {
		direction.normalize();
	}
};

struct RaycastResult {
	Point point;
	QVector3D normal;
	float t;
	bool hit;
};

struct Sphere : public Model
{
	// Point position;
	float radius;

	inline Sphere() : radius(1.0f) { }
	inline Sphere(const Point& p, float r) : radius(r) { position = p; }
};

struct Plane : public Model
{
	// Point position;
	QVector3D normal;

	inline Plane() : normal(1, 0, 0) { }
	inline Plane(const Point& p, const QVector3D& n) : normal(n) { position = p; }
};



// Functions
QVector3D GetMin(const AABB& aabb);
QVector3D GetMax(const AABB& aabb);

// Raycasting
// void ResetRaycastResult(RaycastResult* outResult);
// bool Raycast(const Sphere& sphere, const Ray& ray, RaycastResult* outResult);
// bool Raycast(const AABB& aabb, const Ray& ray, RaycastResult* outResult);
// bool Raycast(const OBB& obb, const Ray& ray, RaycastResult* outResult);
// bool Raycast(const Plane& plane, const Ray& ray, RaycastResult* outResult);
// bool Raycast(const Triangle& triangle, const Ray& ray, RaycastResult* outResult);


// Renderings
// void Render(const Point& point);
void Render(const Line& line);
void Render(const Ray& ray);
void Render(const Sphere& sphere);
// void Render(const AABB& aabb);
// void Render(const OBB& obb);
void Render(const Plane& plane);
// void Render(const Triangle& triangle);
// void Render(const BVHNode& bvh);






