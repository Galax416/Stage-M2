#pragma once

#include "BoundingBox.h"
#include "Rigidbody.h"
#include "Particle.h"
#include "GeometryPrimitives.h"
#include "Intersections.h"

struct TriangleCollider
{
public:
    Particle* p0;
    Particle* p1;
    Particle* p2;

    TriangleCollider(Particle* a, Particle* b, Particle* c)
        : p0(a), p1(b), p2(c) {}

    AABB GetAABB() const {
        QVector3D min = p0->transform.position;
        QVector3D max = min;
        for (Particle* p : {p1, p2}) {
            const QVector3D& pos = p->transform.position;
            min.setX(std::min(min.x(), pos.x()));
            min.setY(std::min(min.y(), pos.y()));
            min.setZ(std::min(min.z(), pos.z()));
            max.setX(std::max(max.x(), pos.x()));
            max.setY(std::max(max.y(), pos.y()));
            max.setZ(std::max(max.z(), pos.z()));
        }
        return FromMinMax(min, max);
    }

    bool Contains(const Particle* p) const {
        return (p0 == p || p1 == p || p2 == p);
    }

};

inline bool CheckParticleTriangleCollision(Particle* p, const TriangleCollider& tri, QVector3D& outCorrection)
{
    QVector3D center = p->transform.position;
    float radius = p->GetRadius();

    QVector3D a = tri.p0->transform.position;
    QVector3D b = tri.p1->transform.position;
    QVector3D c = tri.p2->transform.position;

    QVector3D n = QVector3D::crossProduct(b - a, c - a).normalized();
    float distToPlane = QVector3D::dotProduct(center - a, n);

    if (std::abs(distToPlane) > radius) return false;

    QVector3D projected = center - n * distToPlane;

    QVector3D v0 = b - a;
    QVector3D v1 = c - a;
    QVector3D v2 = projected - a;

    float d00 = QVector3D::dotProduct(v0, v0);
    float d01 = QVector3D::dotProduct(v0, v1);
    float d11 = QVector3D::dotProduct(v1, v1);
    float d20 = QVector3D::dotProduct(v2, v0);
    float d21 = QVector3D::dotProduct(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    if (u >= 0.0f && v >= 0.0f && w >= 0.0f) {
        float penetration = radius - std::abs(distToPlane);
        if (penetration > 0.0f) {
            outCorrection = n * penetration * (distToPlane < 0 ? -1.0f : 1.0f) * 0.5f;
            return true;
        }
    }

    return false;
}

inline bool IsPointInsideMesh(const QVector3D& point, const std::vector<std::shared_ptr<TriangleCollider>>& triangles) {
    int count = 0;
    Ray ray(point, QVector3D(0, 0, 1));

    for (const auto& triangle : triangles) {
        if (RayIntersectsTriangle(ray, triangle)) ++count;
    }

    return count % 2 == 1; // Odd count means inside, even means outside
}

inline bool IsParticleInsideMesh(const std::shared_ptr<Particle> particle, const std::vector<std::shared_ptr<TriangleCollider>>& triangles) {
{
    QVector3D center = particle->GetPosition();
    float radius = particle->GetRadius();

    const QVector3D offsets[] = {
        QVector3D(radius, 0, 0),
        QVector3D(-radius, 0, 0),
        QVector3D(0, radius, 0),
        QVector3D(0, -radius, 0),
        QVector3D(0, 0, radius),
        QVector3D(0, 0, -radius)
    };

    for (const auto& offset : offsets) {
        if (!IsPointInsideMesh(center + offset, triangles))
            return false;
    }

    return true;
}
}