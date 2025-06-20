#include "Rigidbody.h"
#include "Render.h"

#include "Particle.h"
#include "CollisionSolver.h"
#include "TriangleCollider.h"

unsigned int Rigidbody::nextId = 0;

void Rigidbody::Render(QOpenGLShaderProgram* shaderProgram)
{
    shaderProgram->bind();

    // Transform t = transform; // Copy the transform for rendering
    // t.position = displayPosition; // Use display position for rendering

    shaderProgram->setUniformValue("material.albedo", QVector3D(1, 0, 0));
    shaderProgram->setUniformValue("transparency", static_cast<GLfloat>(0.5f)); // 50% transparent
    shaderProgram->setUniformValue("model", transform.GetModelMatrix());
    
    if (type == RIGIDBODY_TYPE_SPHERE || type == RIGIDBODY_TYPE_PARTICLE) ::Render(sphereCollider);
    else if (type == RIGIDBODY_TYPE_BOX) ::Render(boxCollider);
    else if (type == RIGIDBODY_TYPE_TRIANGLE) ::Render(triangleColliders);

    shaderProgram->release();
}

void Rigidbody::ApplyPositionCorrection(const QVector3D& correction) 
{ 
    if (IsStatic()) return;
    transform.position += correction;
    SynsCollisionVolumes();
}

void Rigidbody::ApplyRotationCorrection(const QVector3D& torque)
{
    if (IsStatic()) return;
    
    float torqueMagnitude = torque.length();
    if (torqueMagnitude < 1e-6f) return; // Ignore very small torques

    QVector3D axis = torque.normalized();

    float angle = torqueMagnitude * 0.1f; // Scale the angle for correction, adjust as needed

    QQuaternion rotationCorrection = QQuaternion::fromAxisAndAngle(axis, qRadiansToDegrees(angle));

    transform.SetRotation((transform.rotation * rotationCorrection).normalized());
    SynsCollisionVolumes();
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
        SolvePairCollision(this, tri.get());
    }
}

AABB Rigidbody::GetAABB() const
{
    if (type == RIGIDBODY_TYPE_PARTICLE) {
        return AABB(transform.position, transform.scale);
    }
    else if (type == RIGIDBODY_TYPE_SPHERE) {
        return AABB(sphereCollider.center, QVector3D(sphereCollider.radius, sphereCollider.radius, sphereCollider.radius));
    }
    else if (type == RIGIDBODY_TYPE_BOX) {
        return boxCollider.toAABB();
    } 
    else if (type == RIGIDBODY_TYPE_TRIANGLE) {
        return boxCollider.toAABB();
    } 
    else {
        return AABB(transform.position, QVector3D(0, 0, 0)); // Default case
    } 
}