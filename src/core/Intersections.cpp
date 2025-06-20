#include "Intersections.h"
#include "TriangleCollider.h"

RayCastResult  RayIntersectsTriangle(const Ray& ray, const std::shared_ptr<TriangleCollider>& tri)
{
    RayCastResult result;

    const float EPSILON = 1e-6f;
    
    QVector3D v0, v1, v2;
    if (tri->p0 == nullptr || tri->p1 == nullptr || tri->p2 == nullptr) {
        v0 = tri->pos0;
        v1 = tri->pos1;
        v2 = tri->pos2;
    } else {
        v0 = tri->p0->GetPosition();
        v1 = tri->p1->GetPosition();
        v2 = tri->p2->GetPosition();
    }

    QVector3D e1 = v1 - v0;
    QVector3D e2 = v2 - v0;

    QVector3D h = QVector3D::crossProduct(ray.direction, e2);
    float a = QVector3D::dotProduct(e1, h);
    if (std::abs(a) < EPSILON) return result; // This ray is parallel to this triangle.

    float f = 1.0f / a;
    QVector3D s = ray.origin - v0;
    float u = f * QVector3D::dotProduct(s, h);
    if (u < 0.0f || u > 1.0f) return result; // The intersection point is outside the triangle.

    QVector3D q = QVector3D::crossProduct(s, e1);
    float v = f * QVector3D::dotProduct(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) return result; // The intersection point is outside the triangle.

    float t = f * QVector3D::dotProduct(e2, q);
    if (t > EPSILON) // ray intersection
    {
        result.t = t;
        result.point = ray.origin + ray.direction * t;
        result.normal = QVector3D::crossProduct(e1, e2).normalized();
        result.hit = true;
    }
    return result;
}


RayCastResult  RayIntersectsTriangle(const Ray& ray, const Triangle& tri)
{
    RayCastResult result;

    const float EPSILON = 1e-6f;
    
    QVector3D v0 = tri.a;
    QVector3D v1 = tri.b;
    QVector3D v2 = tri.c;

    QVector3D e1 = v1 - v0;
    QVector3D e2 = v2 - v0;

    QVector3D h = QVector3D::crossProduct(ray.direction, e2);
    float a = QVector3D::dotProduct(e1, h);
    if (std::abs(a) < EPSILON) return result; // This ray is parallel to this triangle.

    float f = 1.0f / a;
    QVector3D s = ray.origin - v0;
    float u = f * QVector3D::dotProduct(s, h);
    if (u < 0.0f || u > 1.0f) return result; // The intersection point is outside the triangle.

    QVector3D q = QVector3D::crossProduct(s, e1);
    float v = f * QVector3D::dotProduct(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) return result; // The intersection point is outside the triangle.

    float t = f * QVector3D::dotProduct(e2, q);
    if (t > EPSILON) // ray intersection
    {
        result.t = t;
        result.point = ray.origin + ray.direction * t;
        result.normal = QVector3D::crossProduct(e1, e2).normalized();
        result.hit = true;
    }
    return result;
}