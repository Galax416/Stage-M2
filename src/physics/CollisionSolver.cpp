#include "CollisionSolver.h"
#include "Particle.h"

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
        a->SolveParticleParticleCollision(*pa, *pb);
    }
    else if (typeA == RIGIDBODY_TYPE_PARTICLE && typeB == RIGIDBODY_TYPE_SPHERE) {
        // auto* pa = static_cast<Particle*>(a);
        // a->SolveParticleSphereCollision(*pa, b->sphereCollider, b);
    }
    else if (typeA == RIGIDBODY_TYPE_PARTICLE && typeB == RIGIDBODY_TYPE_BOX) {
        auto* pa = static_cast<Particle*>(a);
        a->SolveParticleOBBCollision(*pa, b);
    }
    else if (typeA == RIGIDBODY_TYPE_SPHERE && typeB == RIGIDBODY_TYPE_SPHERE) {
        // a->SolveSphereSphereCollision(a->sphereCollider, b->sphereCollider, b);
    }
    else if (typeA == RIGIDBODY_TYPE_SPHERE && typeB == RIGIDBODY_TYPE_BOX) {
        // a->SolveSphereOBBCollision(a->sphereCollider, b->boxCollider, b);
    }
    else if (typeA == RIGIDBODY_TYPE_BOX && typeB == RIGIDBODY_TYPE_BOX) {
        // a->SolveOBBOBBCollision(a->boxCollider, b->boxCollider, b);
    }
}
