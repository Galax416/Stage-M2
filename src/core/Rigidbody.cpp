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

void Rigidbody::AddLinearImpulse(const QVector3D& impulse)
{
    if (!isMovable) return;
    
    QVector3D velocity = GetVelocity();
    velocity += impulse;
    oldPosition = transform.position - velocity;

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
        if (particle->GetFlags() & ParticleFlags::PARTICLE_ATTACHED_TO_TRIANGLE) continue; // Skip if attached to triangle
        // if (tri->Contains(particle)) continue; // Skip if already inside triangle

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

QMatrix4x4 Rigidbody::InvTensor()
{
    if (mass == 0) return QMatrix4x4({0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    });
    float ix = 0.0f, iy = 0.0f, iz = 0.0f, iw = 0.0f;
    if (type == RIGIDBODY_TYPE_SPHERE) {
        float r2 = sphereCollider.radius * sphereCollider.radius;
        float fraction = (2.0f / 5.0f);
        ix = r2 * mass * fraction;
        iy = r2 * mass * fraction;
        iz = r2 * mass * fraction;
        iw = 1.0f;
    }
    else if (type == RIGIDBODY_TYPE_BOX) {
        QVector3D size = boxCollider.size * 2.0f;
        float fraction = (1.0f / 12.0f);
        float x2 = size.x() * size.x();
        float y2 = size.y() * size.y();
        float z2 = size.z() * size.z();
        ix = (y2 + z2) * mass * fraction;
        iy = (x2 + z2) * mass * fraction;
        iz = (x2 + y2) * mass * fraction;
        iw = 1.0f;
    }
    return QMatrix4x4(
        ix, 0, 0, 0,
        0, iy, 0, 0,
        0, 0, iz, 0,
        0, 0, 0, iw
    ).inverted();
}


/*void Rigidbody::SolveSphereSphereCollision(SphereCollider& s1, SphereCollider& s2, Rigidbody* rb)
{
    QVector3D delta = s1.position - s2.position;
    float dist = delta.length();
    float minDist = s1.radius + s2.radius;

    if (dist < minDist && dist > 1e-6f) { // Collision 
        QVector3D normal = delta.normalized();
        float penetration = minDist - dist;

        QVector3D relativeVelocity = GetVelocity() - rb->GetVelocity();
        float velocityAlongNormal = QVector3D::dotProduct(relativeVelocity, normal);

        if (velocityAlongNormal > 0) return; // Avoid double collision

        float restitution = fminf(cor, rb->cor);
        float invMassA = InvMass();
        float invMassB = rb->InvMass();

        float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal / (invMassA + invMassB);
        QVector3D impulse = normal * impulseMagnitude;

        if (isMovable) AddLinearImpulse(impulse * invMassA);
        if (rb->isMovable) rb->AddLinearImpulse(-impulse * invMassB);

        // Position correction
        float totalInvMass = invMassA + invMassB;
        if (totalInvMass > 0.0f) {
            QVector3D correction = normal * penetration * 0.5f; // Apply progressively
            float ratioA = invMassA / totalInvMass;
            float ratioB = invMassB / totalInvMass;
            
            if (isMovable) transform.position += correction * ratioA;
            if (rb->isMovable) rb->transform.position -= correction * ratioB;
        }
    }
}*/

/*void Rigidbody::SolveSphereOBBCollision(SphereCollider& sphere, OBB& box, Rigidbody* rb)
{
    // Convert sphere position to box local space
    QVector3D localSpherePos = QMatrix4x4(box.orientation.transposed()).map(sphere.position - box.position);
    
    // Clamp local sphere position to box half-extents
    QVector3D closestPoint = localSpherePos;
    closestPoint.setX(qBound(-box.size.x(), closestPoint.x(), box.size.x()));
    closestPoint.setY(qBound(-box.size.y(), closestPoint.y(), box.size.y()));
    closestPoint.setZ(qBound(-box.size.z(), closestPoint.z(), box.size.z()));
    
    // Convert back to world space
    QVector3D worldClosestPoint = box.position + QMatrix4x4(box.orientation).map(closestPoint);
    QVector3D delta = sphere.position - worldClosestPoint;
    float dist = delta.length();
    
    if (dist < sphere.radius && dist > 1e-6f) { // Collision detected
        QVector3D normal = delta.normalized();
        float penetration = sphere.radius - dist;
        
        QVector3D relativeVelocity = GetVelocity() - rb->GetVelocity();
        float velocityAlongNormal = QVector3D::dotProduct(relativeVelocity, normal);
        
        if (velocityAlongNormal > 0) return; // Avoid double collision
        
        float restitution = fminf(cor, rb->cor);
        float invMassA = InvMass();
        float invMassB = rb->InvMass();
        
        float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal / (invMassA + invMassB);
        QVector3D impulse = normal * impulseMagnitude;
        
        if (isMovable) AddLinearImpulse(impulse);
        if (rb->isMovable) rb->AddLinearImpulse(-impulse);
        // if (isMovable) AddLinearImpulse(impulse * invMassA);
        // if (rb->isMovable) rb->AddLinearImpulse(-impulse * invMassB);

        // Position correction
        float totalInvMass = invMassA + invMassB;
        if (totalInvMass > 0.0f) {
            QVector3D correction = normal * penetration * 0.5f; // Apply progressively
            float ratioA = invMassA / totalInvMass;
            float ratioB = invMassB / totalInvMass;
            
            if (isMovable) transform.position += correction * ratioA;
            if (rb->isMovable) rb->transform.position -= correction * ratioB;

        }
    }

}*/