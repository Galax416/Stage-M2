#include "Intersections.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"

RayCastResult RayIntersectsSphere(const Ray& ray, const SphereCollider& sphere)
{
    RayCastResult result;

    QVector3D oc = ray.origin - sphere.center;
    float a = QVector3D::dotProduct(ray.direction, ray.direction);
    float b = 2.0f * QVector3D::dotProduct(oc, ray.direction);
    float c = QVector3D::dotProduct(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return result; // No intersection

    float t = (-b - sqrt(discriminant)) / (2.0f * a);
    if (t < 0) return result; // Intersection behind the ray origin

    result.t = t;
    result.point = ray.origin + ray.direction * t;
    result.normal = (result.point - sphere.center).normalized();
    result.hit = true;

    return result;
}

RayCastResult RayIntersectsOBB(const Ray& ray, const OBB& obb)
{
    RayCastResult result;

    // OBB local space transformation
    const QVector3D& center = obb.center;
    const QVector3D& size = obb.size;
    const QMatrix3x3& orientation = obb.orientation;

    QVector3D p = center - ray.origin;
    float tMin = -std::numeric_limits<float>::infinity();
    float tMax = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; ++i) {
        QVector3D col(orientation.constData()[i], orientation.constData()[i + 3], orientation.constData()[i + 6]);
        float e = QVector3D::dotProduct(col, p);
        float f = QVector3D::dotProduct(col, ray.direction);
        if (std::abs(f) > 1e-6f) {
            float t1 = (e + size[i]) / f;
            float t2 = (e - size[i]) / f;
            if (t1 < t2) {
                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);
            } else {
                tMin = std::max(tMin, t2);
                tMax = std::min(tMax, t1);
            }
            if (tMin > tMax) return result; // No intersection
        } else if (-e - size[i] > 0 || -e + size[i] < 0) {
            return result; // No intersection
        }
    }

    result.hit = true;
    result.t = (tMin >= 0.0f) ? tMin : tMax;
    result.point = ray.origin + ray.direction.normalized() * result.t;
    result.normal = (result.point - center).normalized();
    return result;
}


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