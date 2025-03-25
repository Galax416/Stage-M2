#include "Particle.h"

Particle::Particle()
{
    type = RIGIDBODY_TYPE_PARTICLE;
}

Particle::Particle(QVector3D pos, float r, float m, bool mov, QColor c)
{
    type = RIGIDBODY_TYPE_PARTICLE;
    position = pos;//ScreenCordinateToUV(pos);
    oldPosition = position;
    radius = r * 0.01f;
    mass = m;
    movable = mov;
    color = c;
}

void Particle::Update(float deltaTime)
{
    if (!movable) return;

    QVector3D velocity = (position - oldPosition);
	oldPosition = position;
	float deltaSquare = deltaTime * deltaTime;
	position = position + (velocity * friction + forces * deltaSquare);
}

void Particle::Render(QOpenGLShaderProgram* shaderProgram)
{
    // Sphere visual(position, radius);
    // ::Render(visual);

    shaderProgram->setUniformValue("color", color);
    // shaderProgram->setUniformValue("model", QMatrix4x4());

    Sphere visual(position, radius);
    shaderProgram->setUniformValue("model", visual.GetModelMatrix());
    ::Render(visual);


    // Render form
    // Circle visual(QVector2D(position), radius);
    // ::Render(visual);

}

void Particle::ApplyForces()
{
    forces = gravity * mass; // gravity
}

void Particle::AddImpulse(const QVector3D& impulse)
{
    if (!movable) return;
    QVector3D velocity = position - oldPosition;
	velocity = velocity + impulse;
	oldPosition = position - velocity;
}

void Particle::SolveConstraints(const std::vector<Rigidbody*>& constraints) {
    for (Rigidbody* rb : constraints) {
        Particle* other = dynamic_cast<Particle*>(rb);
        if (!other || other == this) continue; 

        QVector3D delta = position - other->GetPosition();
        float dist = delta.length();
        float minDist = radius + other->GetRadius();

        if (dist < minDist && dist > 1e-6f) { // Collision
            QVector3D normal = delta.normalized();
            QVector3D relativeVelocity = GetVelocity() - other->GetVelocity();
            float velocityAlongNormal = QVector3D::dotProduct(relativeVelocity, normal);

            if (velocityAlongNormal > 0) continue; // Avoid double collision
            
            
            float restitution = std::min(bounce, other->GetBounce()); // Prendre le plus faible
            float invMassA = InvMass();
            float invMassB = other->InvMass();
            float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal / (invMassA + invMassB);

            // impulseMagnitude *= 0.1f;

            QVector3D impulse = normal * impulseMagnitude;

            AddImpulse(impulse * invMassA);
            other->AddImpulse(-impulse * invMassB);

            // Position correction
            float totalInvMass = invMassA + invMassB;
            if (totalInvMass > 0.0f) {
                QVector3D correction = normal * (minDist - dist) * 0.2f; // Appliquer progressivement
                float ratioA = invMassA / totalInvMass;
                float ratioB = invMassB / totalInvMass;
                
                if (movable) position += correction * ratioA;
                if (other->movable) other->position -= correction * ratioB;
            }

        }
    }
}
   

