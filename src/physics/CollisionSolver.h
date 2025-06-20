#pragma once

#include <memory>
#include "Rigidbody.h"

void SolvePairCollision(Rigidbody* a, Rigidbody* b);
void SolvePairCollision(Rigidbody* a, TriangleCollider* b);


void SolveParticleParticleCollision(Particle& p1, Particle& p2);
// void SolveParticleSphereCollision(Particle& p, SphereCollider& sphere, Rigidbody* rb) {}
void SolveParticleOBBCollision(Particle& p, Rigidbody* rb);
void SolvePaticleTriangleCollision(Particle& p, TriangleCollider& tri);
// void SolveOBBTriangleCollision(OBB& box, TriangleCollider& tri);
// void SolveSphereSphereCollision(SphereCollider& s1, SphereCollider& s2, Rigidbody* rb) {}
void SolveSphereOBBCollision(Rigidbody* sphereRb, Rigidbody* obbRb);
void SolveOBBOBBCollision(Rigidbody* rb1, Rigidbody* rb2);