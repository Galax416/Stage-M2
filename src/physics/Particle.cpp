#include "Particle.h"
#include "Geometry3D.h"

// Particle::Particle()
// {
//     type = RIGIDBODY_TYPE_PARTICLE;
//     m_flags = PARTICLE_FREE;
//     oldPosition = transform.position;
//     m_radius = 0.01f ; // default radius
//     m_color = Qt::white; // default color
//     cor = 0.2f; // default coefficient of restitution
// }

Particle::Particle(QVector3D pos, float r, float m, bool mov, QColor color)
{
    type = RIGIDBODY_TYPE_PARTICLE;
    transform.position = pos;
    oldPosition = pos;
    m_radius = r * 0.01f;
    mass = m;
    isMovable = mov;
    m_color = color;
    cor = 0.0f;

    m_particleModel = new Sphere(pos, m_radius, m_color);

    sphereCollider.position = transform.position;
    sphereCollider.radius = m_radius;
}

void Particle::Update(float deltaTime)
{
    if (!isMovable) return;

    QVector3D velocity = GetVelocity();
    oldPosition = transform.position;

    float deltaSq = deltaTime * deltaTime;

    // Verlet
    transform.position += (velocity * friction + forces * deltaSq);

    m_particleModel->transform.position = transform.position;
    sphereCollider.position = transform.position;

    forces = QVector3D(0.0f, 0.0f, 0.0f); // Reset forces after applying them

}

void Particle::Render(QOpenGLShaderProgram* shaderProgram)
{
    m_particleModel->Render(shaderProgram);
}

void Particle::SetColor(QColor c)
{
    m_color = c;
    m_particleModel->SetColor(c);
}

void Particle::SetPosition(const QVector3D& p) {
    transform.position = p;
    oldPosition = p;
    m_particleModel->transform.position = p;
}
