#pragma once

#include <QVector3D>
#include "BoundingBox.h"

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