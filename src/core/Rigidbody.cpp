#include "Rigidbody.h"
#include "Render.h"

#include "Particle.h"
#include "CollisionSolver.h"
#include "TriangleCollider.h"

unsigned int Rigidbody::nextId = 0;

void Rigidbody::Render(QOpenGLShaderProgram* shaderProgram)
{
    shaderProgram->bind();

    shaderProgram->setUniformValue("material.albedo", QVector3D(1, 0, 0));
    shaderProgram->setUniformValue("transparency", static_cast<GLfloat>(0.5f)); // 50% transparent
    shaderProgram->setUniformValue("model", transform.GetModelMatrix());
    
    if (type == RIGIDBODY_TYPE_SPHERE) {
        ::Render(sphereCollider);
    }
    else if (type == RIGIDBODY_TYPE_BOX) {
        ::Render(boxCollider);
    }

    shaderProgram->release();
}

void Rigidbody::SolveConstraints(const std::vector<std::shared_ptr<Rigidbody>>& constraints)
{
    for (auto& rb : constraints) {
        if (rb.get() == this) continue;
        SolvePairCollision(this, rb.get());
    }
}

void Rigidbody::SolveConstraints(const std::vector<std::shared_ptr<TriangleCollider>>& constraints)
{
    for (auto& tri : constraints) {
        auto particle = dynamic_cast<Particle*>(this);
        if (!particle) continue; // Only particles can collide with triangles
        if (particle->HasFlag(PARTICLE_ATTACHED_TO_TRIANGLE)) continue; // Skip if attached to triangle
        if (tri->Contains(particle)) continue; // Skip if already inside triangle

        SolvePairCollision(particle, tri.get());
    }
}

AABB Rigidbody::GetAABB() const
{
    if (type == RIGIDBODY_TYPE_PARTICLE) {
        return AABB(transform.position, QVector3D(sphereCollider.radius, sphereCollider.radius, sphereCollider.radius));
    }
    else if (type == RIGIDBODY_TYPE_SPHERE) {
        return AABB(sphereCollider.position, QVector3D(sphereCollider.radius, sphereCollider.radius, sphereCollider.radius));
    }
    else if (type == RIGIDBODY_TYPE_BOX) {
        return boxCollider.toAABB();
    } else {
        return AABB(transform.position, QVector3D(0, 0, 0)); // Default case
    } 
}