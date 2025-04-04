#include "Particle.h"

Particle::Particle()
{
    type = RIGIDBODY_TYPE_PARTICLE;
}

Particle::Particle(QVector3D pos, float r, float m, bool mov, QColor c)
{
    type = RIGIDBODY_TYPE_PARTICLE;
    transform.position = pos;
    oldPosition = pos;
    m_radius = r * 0.01f;
    mass = m;
    isMovable = mov;
    m_color = c;
    cor = m_bounce;

    m_particleModel = new Sphere(pos, m_radius, m_color);
}

void Particle::Update(float deltaTime)
{
    if (!isMovable) return;

    QVector3D velocity = (transform.position - oldPosition);
    // Rigidbody::velocity = velocity;
	oldPosition = transform.position;
	float deltaSquare = deltaTime * deltaTime;
	transform.position += (velocity * friction + forces * deltaSquare);
    m_particleModel->transform.position = transform.position;
}

void Particle::Render(QOpenGLShaderProgram* shaderProgram)
{
    // shaderProgram->setUniformValue("color", m_color);
    m_particleModel->Render(shaderProgram);
}

void Particle::SolveConstraints(const std::vector<Rigidbody*>& constraints) {
    if (!isMovable) return;
    for (Rigidbody* rb : constraints) {
        // Collision with OBB
        if (rb->type == RIGIDBODY_TYPE_BOX) {
            OBB box = rb->boxCollider;

            // Convert sphere position to box local space
            QVector3D localSpherePos = QMatrix4x4(box.orientation.transposed()).map(transform.position - box.position);
            
            // Clamp local sphere position to box half-extents
            QVector3D closestPoint = localSpherePos;
            closestPoint.setX(qBound(-box.size.x(), closestPoint.x(), box.size.x()));
            closestPoint.setY(qBound(-box.size.y(), closestPoint.y(), box.size.y()));
            closestPoint.setZ(qBound(-box.size.z(), closestPoint.z(), box.size.z()));
            
            // Convert back to world space
            QVector3D worldClosestPoint = box.position + QMatrix4x4(box.orientation).map(closestPoint);
            QVector3D delta = transform.position - worldClosestPoint;
            float dist = delta.length();
            
            if (dist < m_radius && dist > 1e-6f) { // Collision detected
                QVector3D normal = delta.normalized();
                float penetration = m_radius - dist;
                
                QVector3D relativeVelocity = GetVelocity() - rb->GetVelocity();
                float velocityAlongNormal = QVector3D::dotProduct(relativeVelocity, normal);
                
                if (velocityAlongNormal > 0) return; // Avoid double collision
                
                float restitution = fminf(cor, rb->cor); // Coefficient of restitution
                float invMassA = InvMass();
                float invMassB = rb->InvMass();
                
                float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal / (invMassA + invMassB);
                QVector3D impulse = normal * impulseMagnitude;

                // impulse *= 10.0f; // Apply progressively
                
                if (isMovable) AddLinearImpulse(impulse);
                if (rb->isMovable) rb->AddLinearImpulse(-impulse);

                // Position correction
                float totalInvMass = invMassA + invMassB;
                if (totalInvMass > 0.0f) {
                    QVector3D correction = normal * penetration * 0.5f; // Apply progressively
                    float ratioA = invMassA / totalInvMass;
                    float ratioB = invMassB / totalInvMass;
                    
                    if (isMovable) {
                        transform.position += correction * ratioA;
                        m_particleModel->transform.position = transform.position;
                    }
                    if (rb->isMovable) {
                        rb->transform.position -= correction * ratioB;
                    }

                }
            }
            continue;
        }
        else if (rb->type == RIGIDBODY_TYPE_SPHERE) {
            
            continue;
        }

        // Collision with another particle
        Particle* other = dynamic_cast<Particle*>(rb);
        if (!other || other == this) continue; 

        QVector3D delta = transform.position - other->transform.position;
        float dist = delta.length();
        float minDist = m_radius + other->GetRadius();

        if (dist < minDist && dist > 1e-6f) { // Collision
            QVector3D normal = delta.normalized();
            QVector3D relativeVelocity = GetVelocity() - other->GetVelocity();
            float velocityAlongNormal = QVector3D::dotProduct(relativeVelocity, normal);

            if (velocityAlongNormal > 0) continue; // Avoid double collision
            
            
            float restitution = fminf(m_bounce, other->GetBounce()); // Coefficient of restitution
            float invMassA = InvMass();
            float invMassB = other->InvMass();
            float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal / (invMassA + invMassB);

            QVector3D impulse = normal * impulseMagnitude;

            AddLinearImpulse(impulse * invMassA);
            other->AddLinearImpulse(-impulse * invMassB);

            // Position correction
            float totalInvMass = invMassA + invMassB;
            if (totalInvMass > 0.0f) {
                QVector3D correction = normal * (minDist - dist) * 0.5f; // Apply progressively
                float ratioA = invMassA / totalInvMass;
                float ratioB = invMassB / totalInvMass;
                
                if (isMovable) {
                    transform.position += correction * ratioA;
                    m_particleModel->transform.position = transform.position;
                }
                if (other->isMovable) {
                    other->transform.position -= correction * ratioB;
                    other->m_particleModel->transform.position = other->transform.position;
                }
            }
        }
    }
}
   

