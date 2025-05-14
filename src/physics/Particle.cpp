#include "Particle.h"
#include "Geometry3D.h"

Particle::Particle(QVector3D pos, float r, float m, bool isDynamic, QColor color)
    : m_radius(r * 0.01f), m_flags(PARTICLE_FREE)
{
    type = RIGIDBODY_TYPE_PARTICLE;

    m_particleModel = std::make_unique<Sphere>(pos, m_radius, color);
    
    SetPosition(pos);
    SetMass(m);
    isDynamic ? SetDynamic() : SetStatic();

    SynsCollisionVolumes();
}

void Particle::Update(float dt)
{
    // Optional: XPBD doesn't use this unless needed
}

void Particle::Render(QOpenGLShaderProgram* shaderProgram)
{
    m_particleModel->Render(shaderProgram);
}

void Particle::SynsCollisionVolumes()
{
    sphereCollider.position = transform.position;
    sphereCollider.radius = m_radius;
}

void Particle::SetColor(QColor c)
{
    m_particleModel->SetColor(c);
}

QColor Particle::GetColor() const
{
    return m_particleModel->GetColor();
}

void Particle::SetPosition(const QVector3D& p) {
    transform.position = p;
    oldPosition = p;
    m_particleModel->transform.position = p;

    SynsCollisionVolumes();
}

void Particle::SetRotation(const QQuaternion& q) {
    transform.rotation = q;
    m_particleModel->transform.rotation = q;

    SynsCollisionVolumes();
}
