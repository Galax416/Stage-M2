#include "Intersections.h"
#include "TriangleCollider.h"

bool RayIntersectsTriangle(const Ray& ray, const std::shared_ptr<TriangleCollider>& tri)
{
    const float EPSILON = 1e-6f;
    QVector3D v0 = tri->p0->GetPosition();
    QVector3D v1 = tri->p1->GetPosition();
    QVector3D v2 = tri->p2->GetPosition();

    QVector3D e1 = v1 - v0;
    QVector3D e2 = v2 - v0;

    QVector3D h = QVector3D::crossProduct(ray.direction, e2);
    float a = QVector3D::dotProduct(e1, h);
    if (std::abs(a) < EPSILON) return false; // This ray is parallel to this triangle.

    float f = 1.0f / a;
    QVector3D s = ray.origin - v0;
    float u = f * QVector3D::dotProduct(s, h);
    if (u < 0.0f || u > 1.0f) return false; // The intersection point is outside the triangle.

    QVector3D q = QVector3D::crossProduct(s, e1);
    float v = f * QVector3D::dotProduct(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) return false; // The intersection point is outside the triangle.

    float t = f * QVector3D::dotProduct(e2, q);
    return t > EPSILON; // ray intersection
}