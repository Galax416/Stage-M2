#pragma once

#include <QOpenGLFunctions>
#include <QVector2D>
#include <QVector3D>
#include <QtMath>
#include <QDebug>
#include <qgenericmatrix.h>

#define PI 3.14159265358979323846f


// 2D Geometry
typedef QVector2D Point2D;

typedef struct Circle 
{
	Point2D position;
	float radius;

	inline Circle() : radius(1.0f) {}
	inline Circle(const Point2D& p, float r):
		position(p), radius(r) {}
} Circle;

// Renderings 2D
void Render(const Circle& circle);

// 3D Geometry
typedef QVector3D Point;

typedef struct Line 
{
	Point start;
	Point end;

	inline Line() {}
	inline Line(const Point& s, const Point& e) :
		start(s), end(e) { }
} Line;

typedef struct Ray 
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
} Ray;

typedef struct RaycastResult {
	QVector3D point;
	QVector3D normal;
	float t;
	bool hit;
} RaycastResult;

typedef struct Sphere 
{
	Point position;
	float radius;

	inline Sphere() : radius(1.0f) { }
	inline Sphere(const Point& p, float r) :
		position(p), radius(r) { }
} Sphere;

typedef struct AABB {
	Point position;
	QVector3D size; // HALF SIZE!

	inline AABB() : size(1, 1, 1) { }
	inline AABB(const Point& p, const QVector3D& s) :
		position(p), size(s) { }
} AABB;

typedef struct OBB {
	Point position;
	QVector3D size; // HALF SIZE!
	QMatrix3x3 orientation;

	inline OBB() : size(1, 1, 1) { }
	inline OBB(const Point& p, const QVector3D& s) :
		position(p), size(s) { }
	inline OBB(const Point& p, const QVector3D& s, const QMatrix3x3& o) :
		position(p), size(s), orientation(o) { }
} OBB;

// Renderings 3D
// void Render(const Point& point);
void Render(const Line& line);
void Render(const Ray& ray);
void Render(const Sphere& sphere);
void Render(const AABB& aabb);
void Render(const OBB& obb);
