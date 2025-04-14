#pragma once

#include <memory>
#include "Rigidbody.h"

void SolvePairCollision(Rigidbody* a, Rigidbody* b);
void SolvePairCollision(Particle* a, TriangleCollider* b);


void SolveParticleParticleCollision(Particle& p1, Particle& p2);
// void SolveParticleSphereCollision(Particle& p, SphereCollider& sphere, Rigidbody* rb) {}
void SolveParticleOBBCollision(Particle& p, Rigidbody* rb);
void SolvePaticleTriangleCollision(Particle& p, TriangleCollider& tri);
// void SolveSphereSphereCollision(SphereCollider& s1, SphereCollider& s2, Rigidbody* rb) {}
// void SolveSphereOBBCollision(SphereCollider& sphere, OBB& box, Rigidbody* rb) {}
// void SolveOBBOBBCollision(OBB& box1, OBB& box2, Rigidbody* rb) {}