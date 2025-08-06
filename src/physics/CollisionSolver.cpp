#include "CollisionSolver.h"
#include "Particle.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"

// Dispatcher logic
void SolvePairCollision(Rigidbody* a, Rigidbody* b)
{
    if (!a || !b) return;

    int typeA = a->GetType();
    int typeB = b->GetType();

    // Avoid handling same pair twice
    if (typeA > typeB) std::swap(a, b);
    typeA = a->GetType();
    typeB = b->GetType();

    if (typeA == RIGIDBODY_TYPE_PARTICLE && typeB == RIGIDBODY_TYPE_PARTICLE) {
        auto* pa = static_cast<Particle*>(a);
        auto* pb = static_cast<Particle*>(b);
        if (pa->HasFlag(PARTICLE_NO_COLLISION_WITH_US) && 
            pb->HasFlag(PARTICLE_NO_COLLISION_WITH_US)) return; // Skip if no collision with us
        if (pa->HasFlag(PARTICLE_ATTACHED_TO_TRIANGLE) && 
            pb->HasFlag(PARTICLE_ATTACHED_TO_TRIANGLE)) return; // Skip if attached to triangle
        SolveParticleParticleCollision(*pa, *pb);
    }
    else if (typeA == RIGIDBODY_TYPE_PARTICLE && typeB == RIGIDBODY_TYPE_SPHERE) {
        // auto* pa = static_cast<Particle*>(a);
        // SolveParticleSphereCollision(*pa, b->sphereCollider, b);
    }
    else if (typeA == RIGIDBODY_TYPE_PARTICLE && typeB == RIGIDBODY_TYPE_BOX) {
        auto* pa = static_cast<Particle*>(a);
        SolveParticleOBBCollision(*pa, b);
    }
    else if (typeA == RIGIDBODY_TYPE_SPHERE && typeB == RIGIDBODY_TYPE_SPHERE) {
        // SolveSphereSphereCollision(a->sphereCollider, b->sphereCollider, b);
    }
    else if (typeA == RIGIDBODY_TYPE_SPHERE && typeB == RIGIDBODY_TYPE_BOX) {
        SolveSphereOBBCollision(a, b);
    }
    else if (typeA == RIGIDBODY_TYPE_BOX && typeB == RIGIDBODY_TYPE_BOX) {
        SolveOBBOBBCollision(a, b);
    }
}

void SolvePairCollision(Rigidbody* a, TriangleCollider* b)
{
    if (!a || !b) return;

    int typeA = a->GetType();

    if (typeA == RIGIDBODY_TYPE_PARTICLE) {
        auto* pa = static_cast<Particle*>(a);
        // if (pa->HasFlag(PARTICLE_NO_COLLISION_WITH_US)) return; // Skip if no collision with us
        // if (pa->HasFlag(PARTICLE_ATTACHED_TO_TRIANGLE)) return; // Skip if attached to triangle
        SolveParticleTriangleCollision(*pa, *b);
    }

}


void SolveParticleParticleCollision(Particle& p1, Particle& p2)
{
    QVector3D delta = p1.GetPosition() - p2.GetPosition();
    float dist = delta.length();
    float minDist = p1.GetRadius() + p2.GetRadius();

    if (dist < 1e-6f ||dist >= minDist) return; // Ignore very small distances and no collision 

    float w1 = p1.GetInvMass();
    float w2 = p2.GetInvMass();
    float totalInvMass = w1 + w2;
    if (totalInvMass <= 0.0f) return; // Ignore static objects

    float penetration = minDist - dist;
    if (penetration < 1e-6f) return; // Ignore very small penetrations

    QVector3D correction = delta.normalized() * (penetration / totalInvMass);

    if (p1.IsDynamic()) p1.ApplyPositionCorrection(correction * w1);
    if (p2.IsDynamic()) p2.ApplyPositionCorrection(-correction * w2);

}

void SolveParticleOBBCollision(Particle& p, Rigidbody* rb)
{
    QVector3D pos = p.GetPosition();
    QVector3D closestPoint = rb->boxCollider.ClosestPoint(pos);
    QVector3D delta = pos - closestPoint;
    float dist = delta.length();
    float radius = p.GetRadius();

    if (dist < 1e-6f || dist >= radius) return; // Ignore very small distances and no collision

    float w1 = p.GetInvMass();
    float w2 = rb->GetInvMass();
    float totalInvMass = w1 + w2;
    if (totalInvMass <= 0.0f) return; // Ignore static objects

    float penetration = radius - dist;
    if (penetration < 1e-6f) return; // Ignore very small penetrations

    QVector3D correction = delta.normalized() * (penetration / totalInvMass);

    if (p.IsDynamic()) p.ApplyPositionCorrection(correction * w1);
    if (rb->IsDynamic()) rb->ApplyPositionCorrection(-correction * w2);

}

void SolveParticleTriangleCollision(Particle& p, TriangleCollider& tri)
{
    float w = p.GetInvMass();

    float w1 = tri.p0 ? tri.p0->GetInvMass() : 1.0f;
    float w2 = tri.p1 ? tri.p1->GetInvMass() : 1.0f;
    float w3 = tri.p2 ? tri.p2->GetInvMass() : 1.0f;

    float totalInvMass = w + w1 + w2 + w3;
    if (totalInvMass <= 0.0f) return; // Ignore static objects

    QVector3D posStart = p.oldPosition;
    QVector3D posEnd   = p.GetPosition();
    QVector3D dir = (posEnd - posStart);

    // CCD (Continuous Collision Detection)
    Ray ray(posStart, dir);
    auto triangleCollider = std::make_shared<TriangleCollider>(tri);
    RayCastResult ccdResult = RayIntersectsTriangle(ray, triangleCollider);

    if (ccdResult.hit && ccdResult.t >= 0.0f && ccdResult.t <= dir.length()) {

        QVector3D corrected = ccdResult.point - ray.direction.normalized() * p.GetRadius();
        QVector3D correction = corrected - posEnd;

        correction /= totalInvMass;

        if (p.IsDynamic()) { p.ApplyPositionCorrection(correction * w); }

        if (tri.p0 != nullptr && tri.p1 != nullptr && tri.p2 != nullptr) {
            if (tri.p0->IsDynamic()) { tri.p0->ApplyPositionCorrection(-correction * w1); }
            if (tri.p1->IsDynamic()) { tri.p1->ApplyPositionCorrection(-correction * w2); }
            if (tri.p2->IsDynamic()) { tri.p2->ApplyPositionCorrection(-correction * w3); }
        }
    }

    // DCD (Discrete Collision Detection) fallback
    QVector3D correction;
    if (CheckParticleTriangleCollision(&p, tri, correction)) {
        correction /= totalInvMass;

        if (p.IsDynamic()) { p.ApplyPositionCorrection(correction * w); }

        if (tri.p0 != nullptr && tri.p1 != nullptr && tri.p2 != nullptr) {
            if (tri.p0->IsDynamic()) { tri.p0->ApplyPositionCorrection(-correction * w1); }
            if (tri.p1->IsDynamic()) { tri.p1->ApplyPositionCorrection(-correction * w2); }
            if (tri.p2->IsDynamic()) { tri.p2->ApplyPositionCorrection(-correction * w3); }
        }
    } 
}

void SolveSphereOBBCollision(Rigidbody* sphereRb, Rigidbody* obbRb)
{
    if (!sphereRb || !obbRb) return;

    QVector3D sphereCenter = sphereRb->sphereCollider.center;
    float sphereRadius = sphereRb->sphereCollider.radius;

    QVector3D closestPoint = obbRb->boxCollider.ClosestPoint(sphereCenter);
    QVector3D delta = sphereCenter - closestPoint;
    float dist = delta.length();

    if (dist < 1e-6f || dist >= sphereRadius) return;

    float w1 = sphereRb->GetInvMass();
    float w2 = obbRb->GetInvMass();
    float totalInvMass = w1 + w2;
    if (totalInvMass <= 0.0f) return; // Ignore static objects

    float penetration = sphereRadius - dist;
    QVector3D correction = delta.normalized() * (penetration / totalInvMass);

    if (sphereRb->IsDynamic()) sphereRb->ApplyPositionCorrection(correction * w1);
    if (obbRb->IsDynamic()) obbRb->ApplyPositionCorrection(-correction * w2);
}

void SolveOBBOBBCollision(Rigidbody* rb1, Rigidbody* rb2)
{
    // if (!rb1 || !rb2) return;

    // QVector3D mtv; // Minimum Translation Vector
    // if (!TestOBBOBBCollision(rb1->boxCollider, rb2->boxCollider, mtv)) return; // No collision
    
    // qDebug() << "OBB Collision Detected! MTV:" << mtv;
    // float w1 = rb1->GetInvMass();
    // float w2 = rb2->GetInvMass();
    // float totalInvMass = w1 + w2;
    // if (totalInvMass <= 0.0f) return; // Ignore static objects

    // QVector3D correction = QVector3D(0, 0.1, 0);// mtv / totalInvMass;

    // if (rb1->IsDynamic()) rb1->ApplyPositionCorrection(correction * w1);
    // if (rb2->IsDynamic()) rb2->ApplyPositionCorrection(-correction * w2);
}
