#pragma once

#include <QVector3D>
#include "BoundingBox.h"
#include "Camera.h"

struct Line
{
    QVector3D start;
    QVector3D end;

    inline Line() {}
    inline Line(const QVector3D& s, const QVector3D& e) : start(s), end(e) { }
};

struct Ray
{
    QVector3D origin;
    QVector3D direction;

    inline Ray() {}
    inline Ray(const QVector3D& o, const QVector3D& d) : origin(o), direction(d) { NormalizeDirection(); }
    inline void NormalizeDirection() { direction.normalize(); }
};

inline Ray ScreenToWorldRay(const QPoint& mousePos, const Camera* camera, int width, int height) 
{
    Ray result;
    result.origin = camera->GetPosition() + camera->GetDownVector() * 0.01f;

    float x = (2.0f * mousePos.x()) / width - 1.0f;
    float y = 1.0f - (2.0f * mousePos.y()) / height;
    float z = 1.0f;
    QVector3D ray_nds(x, y, z);

    QVector4D ray_clip(ray_nds.x(), ray_nds.y(), -1.0, 1.0);
    QVector4D ray_eye = camera->GetProjectionMatrix().inverted() * ray_clip;
    ray_eye.setZ(-1.0);
    ray_eye.setW(0.0);

    QVector4D ray_world = camera->GetViewMatrix().inverted() * ray_eye;
    result.direction = ray_world.toVector3D().normalized();
    return result;
}

struct RayCastResult
{
    QVector3D point;
    QVector3D normal;
    float t;
    bool hit { false };
};

inline static AABB FromRay(const Ray& ray, float epsilon)
{
    QVector3D minPt = ray.origin - QVector3D(epsilon, epsilon, epsilon);
    QVector3D maxPt = ray.origin + ray.direction * 1000.0f + QVector3D(epsilon, epsilon, epsilon);
    return AABB(minPt, maxPt);
}