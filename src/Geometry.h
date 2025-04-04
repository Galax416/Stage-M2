#pragma once

#include <QOpenGLFunctions>
#include <QVector3D>
#include <QVector2D>
#include <vector>

#include "Constants.h"
#include "Model.h"


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
	void SetUpSphere();

	inline Sphere() { color = Qt::gray; SetUpSphere(); }
	inline Sphere(const QVector3D& p, float r) { transform.position = p; transform.scale = QVector3D(r,r,r); color = Qt::gray; SetUpSphere(); }
	inline Sphere(const QVector3D& p, float r, QColor c) { transform.position = p; transform.scale = QVector3D(r,r,r); color = c; SetUpSphere(); }
};

struct Plane : public Model
{
	QVector3D normal;
	float distance;

	void SetUpPlane();
	
	inline Plane() : normal(0, 1, 0) { color = Qt::gray; SetUpPlane(); }
	inline Plane(const QVector3D& n, float d) : normal(n), distance(d) { }
	inline Plane(const QVector3D& p, const QVector3D& n) : normal(n.normalized()) { transform.position = p; color = Qt::gray; SetUpPlane(); }
	inline Plane(const QVector3D& p, const QVector3D& n, QColor c) : normal(n.normalized()) { transform.position = p; color = c; SetUpPlane(); }
};


/*
// Functions
Plane FromTriangle(const Triangle& t);
QVector3D Project(const QVector3D& a, const QVector3D& b);
QVector3D Barycentric(const Point& p, const Triangle& t);


// Collision Detection
bool SphereSphere(const Sphere& A, const Sphere& B);
bool PointInOBB(const Point& point, const OBB& obb);
Point ClosestPoint(const OBB& obb, const Point& point);
std::vector<Point> GetVertices(const OBB& obb);
std::vector<Line> GetEdges(const OBB& obb);
std::vector<Plane> GetPlanes(const OBB& obb);
bool ClipToPlane(const Plane& plane, const Line& line, Point* outPoint);
std::vector<Point> ClipEdgesToOBB(const std::vector<Line>& edges, const OBB& obb);
float PenetrationDepth(const OBB& o1, const OBB& o2, const QVector3D& axis, bool* outShouldFlip);
CollisionManifold FindCollisionFeatures(const Sphere& A, const Sphere& B);
CollisionManifold FindCollisionFeatures(const OBB& A, const Sphere& B);
CollisionManifold FindCollisionFeatures(const OBB& A, const OBB& B);


// Raycasting
void ResetRaycastResult(RaycastResult* outResult);
bool Raycast(const Sphere& sphere, const Ray& ray, RaycastResult* outResult);
bool Raycast(const AABB& aabb, const Ray& ray, RaycastResult* outResult);
bool Raycast(const OBB& obb, const Ray& ray, RaycastResult* outResult);
bool Raycast(const Plane& plane, const Ray& ray, RaycastResult* outResult);
bool Raycast(const Triangle& triangle, const Ray& ray, RaycastResult* outResult);
*/

// Renderings
// void Render(const Point& point);
void Render(const Line& line);
void Render(const Ray& ray);






