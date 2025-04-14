#include "CollisionSolver.h"
#include "Particle.h"
#include "SphereCollider.h"
#include "TriangleCollider.h"

// Dispatcher logic
void SolvePairCollision(Rigidbody* a, Rigidbody* b)
{
    if (!a || !b) return;

    int typeA = a->type;
    int typeB = b->type;

    // Avoid handling same pair twice
    if (typeA > typeB) std::swap(a, b);
    typeA = a->type;
    typeB = b->type;

    if (typeA == RIGIDBODY_TYPE_PARTICLE && typeB == RIGIDBODY_TYPE_PARTICLE) {
        auto* pa = static_cast<Particle*>(a);
        auto* pb = static_cast<Particle*>(b);
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
        // SolveSphereOBBCollision(a->sphereCollider, b->boxCollider, b);
    }
    else if (typeA == RIGIDBODY_TYPE_BOX && typeB == RIGIDBODY_TYPE_BOX) {
        // SolveOBBOBBCollision(a->boxCollider, b->boxCollider, b);
    }
}

void SolvePairCollision(Particle* a, TriangleCollider* b)
{
    if (!a || !b) return;
    SolvePaticleTriangleCollision(*a, *b);
}


void SolveParticleParticleCollision(Particle& p1, Particle& p2)
{
    QVector3D delta = p1.transform.position - p2.transform.position;
    float distSq = delta.lengthSquared();
    float minDist = p1.GetRadius() + p2.GetRadius();

    if (distSq < minDist * minDist && distSq > 1e-8f) { // Collision 
        float dist = std::sqrt(distSq);
        QVector3D normal = delta / dist;
        float penetration = minDist - dist;

        float invMassA = p1.InvMass();
        float invMassB = p2.InvMass();
        float totalInvMass = invMassA + invMassB;

        // Position correction
        if (totalInvMass > 0.0f) {
            QVector3D correction = normal * penetration;
            float ratioA = invMassA / totalInvMass;
            float ratioB = invMassB / totalInvMass;
            
            if (p1.isMovable) p1.transform.position += correction * ratioA;
            if (p2.isMovable) p2.transform.position -= correction * ratioB;

        }
        
        QVector3D relativeVelocity = p1.GetVelocity() - p2.GetVelocity();
        float velocityAlongNormal = QVector3D::dotProduct(relativeVelocity, normal);

        if (velocityAlongNormal > 0) return; // Avoid double collision

        float restitution = fminf(p1.GetCor(), p2.GetCor());
        if (std::abs(velocityAlongNormal) < 0.1f) restitution = 0.0f; // Prevent sticking

        float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal / totalInvMass;
        QVector3D impulse = normal * impulseMagnitude;

        if (p1.isMovable) p1.AddLinearImpulse( impulse * invMassA);
        if (p2.isMovable) p2.AddLinearImpulse(-impulse * invMassB);

        
    }
}
void SolveParticleOBBCollision(Particle& p, Rigidbody* rb)
{
    OBB box = rb->boxCollider;
    // Convert particle position to box local space
    QVector3D localParticlePos = QMatrix4x4(box.orientation.transposed()).map(p.transform.position - box.position);
    
    // Clamp local particle position to box half-extents
    QVector3D closestPoint = localParticlePos;
    closestPoint.setX(qBound(-box.size.x(), closestPoint.x(), box.size.x()));
    closestPoint.setY(qBound(-box.size.y(), closestPoint.y(), box.size.y()));
    closestPoint.setZ(qBound(-box.size.z(), closestPoint.z(), box.size.z()));
    
    // Convert back to world space
    QVector3D worldClosestPoint = box.position + QMatrix4x4(box.orientation).map(closestPoint);
    QVector3D delta = p.transform.position - worldClosestPoint;
    float distSq = delta.lengthSquared();
    float radius = p.GetRadius();
    
    if (distSq < radius * radius && distSq > 1e-8f) { // Collision detected
        float dist = std::sqrt(distSq);
        QVector3D normal = delta / dist;
        float penetration = radius - dist;
        
        float invMassA = p.InvMass();
        float invMassB = rb->InvMass();
        float totalInvMass = invMassA + invMassB;
        
        // Position correction
        if (totalInvMass > 0.0f) {
            QVector3D correction = normal * penetration;
            float ratioA = invMassA / totalInvMass;
            float ratioB = invMassB / totalInvMass;
            
            if (p.isMovable) p.transform.position += correction * ratioA;
            if (rb->isMovable) rb->transform.position -= correction * ratioB;

        }

        QVector3D relativeVelocity = p.GetVelocity() - rb->GetVelocity();
        float velocityAlongNormal = QVector3D::dotProduct(relativeVelocity, normal);
        
        if (velocityAlongNormal > 0) return; // Avoid double collision
        
        float restitution = fminf(p.GetCor(), rb->GetCor());
        if (std::abs(velocityAlongNormal) < 0.1f) restitution = 0.0f; // Prevent sticking
        
        float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal / totalInvMass;
        QVector3D impulse = normal * impulseMagnitude;
        
        // if (isMovable) p.AddLinearImpulse(impulse);
        // if (rb->isMovable) rb->AddLinearImpulse(-impulse);
        if (p.isMovable) p.AddLinearImpulse(impulse * invMassA);
        if (rb->isMovable) rb->AddLinearImpulse(-impulse * invMassB);

    }
}

void SolvePaticleTriangleCollision(Particle& p, TriangleCollider& tri)
{
    // Check if pa is a vertex of the triangle
    if (&p == tri.p0 || &p == tri.p1 || &p == tri.p2) return;

    QVector3D correction;
    if (CheckParticleTriangleCollision(&p, tri, correction)) {
        if (p.isMovable) p.transform.position += correction;
        
    }
}