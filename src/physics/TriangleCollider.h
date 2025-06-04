#pragma once

#include "BoundingBox.h"
#include "Rigidbody.h"
#include "Particle.h"
#include "GeometryPrimitives.h"
#include "Intersections.h"

struct TriangleCollider
{
public:
    std::shared_ptr<Particle> p0;
    std::shared_ptr<Particle> p1;
    std::shared_ptr<Particle> p2;

    QVector3D pos0, pos1, pos2;

    TriangleCollider(std::shared_ptr<Particle> a, std::shared_ptr<Particle> b, std::shared_ptr<Particle> c)
        : p0(a), p1(b), p2(c) {}

    TriangleCollider(const QVector3D& a, const QVector3D& b, const QVector3D& c)
        : p0(nullptr), p1(nullptr), p2(nullptr), 
          pos0(a), pos1(b), pos2(c) {}

    AABB GetAABB() const {
        if ( p0 == nullptr || p1 == nullptr || p2 == nullptr) 
        {
            QVector3D minPos(
                std::min({pos0.x(), pos1.x(), pos2.x()}),
                std::min({pos0.y(), pos1.y(), pos2.y()}),
                std::min({pos0.z(), pos1.z(), pos2.z()})
            );
            QVector3D maxPos(
                std::max({pos0.x(), pos1.x(), pos2.x()}),
                std::max({pos0.y(), pos1.y(), pos2.y()}),
                std::max({pos0.z(), pos1.z(), pos2.z()})
            );
            return FromMinMax(minPos, maxPos);
        }
        else 
        {
            QVector3D min = p0->GetPosition();
            QVector3D max = min;
            for (std::shared_ptr<Particle> p : {p1, p2}) {
                const QVector3D& pos = p->GetPosition();
                min.setX(std::min(min.x(), pos.x()));
                min.setY(std::min(min.y(), pos.y()));
                min.setZ(std::min(min.z(), pos.z()));
                max.setX(std::max(max.x(), pos.x()));
                max.setY(std::max(max.y(), pos.y()));
                max.setZ(std::max(max.z(), pos.z()));
            }
            return FromMinMax(min, max);
        }    

    }

    bool Contains(const Particle* p) const {
        return (p0.get() == p || p1.get() == p || p2.get() == p);
    }

    QVector3D GetCenter() const {
        if (p0 == nullptr || p1 == nullptr || p2 == nullptr) {
            return (pos0 + pos1 + pos2) / 3.0f;
        } else {
            return (p0->GetPosition() + p1->GetPosition() + p2->GetPosition()) / 3.0f;
        }
    }

    float DistanceTo(const QVector3D& point) const {
        return (point - GetCenter()).length();  
    }

};

inline QVector3D ClosestPointOnTriangle(const QVector3D& point, const TriangleCollider& tri)
{
    QVector3D a, b, c;
    if (tri.p0 == nullptr || tri.p1 == nullptr || tri.p2 == nullptr) {
        a = tri.pos0;
        b = tri.pos1;
        c = tri.pos2;
    } else {
        a = tri.p0->GetPosition();
        b = tri.p1->GetPosition();
        c = tri.p2->GetPosition();
    }

    QVector3D ab = b - a;
    QVector3D ac = c - a;
    QVector3D ap = point - a;

    float d1 = QVector3D::dotProduct(ab, ap);
    float d2 = QVector3D::dotProduct(ac, ap);

    if (d1 <= 0.0f && d2 <= 0.0f) return a; // Vertex A

    QVector3D bp = point - b;
    float d3 = QVector3D::dotProduct(ab, bp);
    float d4 = QVector3D::dotProduct(ac, bp);

    if (d3 >= 0.0f && d4 <= d3) return b; // Vertex B

    QVector3D cp = point - c;
    float d5 = QVector3D::dotProduct(ab, cp);
    float d6 = QVector3D::dotProduct(ac, cp);

    if (d6 >= 0.0f && d5 <= d6) return c; // Vertex C


    float vc = d1 * d6 - d5 * d2;
    if (vc <= 0 && d1 >= 0 && d5 <= 0) {
        float v = d1 / (d1 - d5);
        return a + ab * v;
    }

    float vb = d3 * d2 - d1 * d4;
    if (vb <= 0 && d3 >= 0 && d4 <= 0) {
        float v = d3 / (d3 - d4);
        return b + (c - b) * v;
    }

    float va = d5 * d4 - d3 * d6;
    if (va <= 0 && (d6 - d5) >= 0 && (d4 - d3) >= 0) {
        float v = (d6 - d5) / ((d6 - d5) + (d4 - d3));
        return c + (a - c) * v;
    }

    // Inside face region
    float denom = 1.0f / (va + vb + vc);
    float u = vb * denom;
    float v = vc * denom;
    return a + ab * u + ac * v;
}

inline bool CheckParticleTriangleCollision(Particle* p, const TriangleCollider& tri, QVector3D& outCorrection)
{
    QVector3D center = p->GetPosition();
    float radius = p->GetRadius();

    QVector3D a, b, c;
    if (tri.p0 == nullptr || tri.p1 == nullptr || tri.p2 == nullptr) {
        a = tri.pos0;
        b = tri.pos1;
        c = tri.pos2;
    } else {
        a = tri.p0->GetPosition();
        b = tri.p1->GetPosition();
        c = tri.p2->GetPosition();
    }

    QVector3D n = QVector3D::crossProduct(b - a, c - a).normalized();
    float distToPlane = QVector3D::dotProduct(center - a, n);

    if (std::abs(distToPlane) < 1e-6f || std::abs(distToPlane) >= radius) return false;

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
        if (penetration > 1e-6f) {
            outCorrection = n * penetration * (distToPlane < 0 ? -1.0f : 1.0f);
            return true;
        }
    }

    return false;
}

inline bool IsPointInsideMesh(const QVector3D& point, const std::vector<std::shared_ptr<TriangleCollider>>& triangles) 
{
    int count = 0;
    Ray ray(point, QVector3D(0, 0, 1));

    for (const auto& triangle : triangles) {
        RayCastResult result = RayIntersectsTriangle(ray, triangle);
        if (result.hit) ++count;
        // if (RayIntersectsTriangle(ray, triangle)) ++count;

    }

    return count % 2 == 1; // Odd count means inside, even means outside
}

inline bool IsParticleInsideMesh(const std::shared_ptr<Particle> particle, const std::vector<std::shared_ptr<TriangleCollider>>& triangles)
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
        if (!IsPointInsideMesh(center + offset, triangles)) return false;
    }
    return true;
}