#include "Particle.h"
#include "Geometry3D.h"

Particle::Particle(const QVector3D pos, float r, float m, bool isDynamic, QColor color)
    : m_radius(r * 0.01f), m_flags(PARTICLE_FREE)
{
    type = RIGIDBODY_TYPE_PARTICLE;

    m_particleModel = std::make_unique<Sphere>(pos, m_radius, color);
    
    SetPosition(pos);
    // oldPosition = pos;
    // currentPosition = pos;
    // previousPosition = pos;
    // displayPosition = pos;
    transform.scale = QVector3D(m_radius, m_radius, m_radius);
    SetMass(m);
    isDynamic ? SetDynamic() : SetStatic();

    SyncCollisionVolumes();
}

void Particle::ReleaseGLResources()
{
    if (m_particleModel) m_particleModel->ReleaseGLResources();
}

void Particle::Update(float dt)
{
    // Optional: XPBD doesn't use this unless needed
}

void Particle::Render(QOpenGLShaderProgram* shaderProgram)
{
    // m_particleModel->displayPosition = displayPosition;
    m_particleModel->Render(shaderProgram);
}

void Particle::SyncCollisionVolumes()
{
    sphereCollider.center = transform.position;
    sphereCollider.radius = m_radius;
    m_particleModel->SyncCollisionVolumes();
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
    m_particleModel->SetPosition(p);

    SyncCollisionVolumes();
}

void Particle::SetRotation(const QQuaternion& q) {
    transform.SetRotation(q);
    m_particleModel->transform.SetRotation(q);

    SyncCollisionVolumes();
}
