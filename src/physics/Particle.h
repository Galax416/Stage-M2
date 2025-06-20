#pragma once    

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QColor>
#include <vector>
#include <memory>

#include "Rigidbody.h"

enum ParticleFlags
{
    PARTICLE_FREE = 0,
    PARTICLE_ATTACHED_TO_TRIANGLE = 1 << 0,
    PARTICLE_NO_COLLISION_WITH_US = 1 << 1
};

// Forward declaration
class Model; 


class Particle : public Rigidbody
{
private:
    float m_radius { 0.1f }; // default radius
    std::shared_ptr<Model> m_particleModel; // Model for rendering
    ParticleFlags m_flags { PARTICLE_FREE }; // Flags for particle state

public:
    // Particle();
    Particle(QVector3D pos, float r, float m, bool isDynamic = true, QColor color = Qt::white);
    void ReleaseGLResources();

    void Update(float dt) override;
    void Render(QOpenGLShaderProgram* shaderProgram) override;

    void SynsCollisionVolumes() override;

    void SetPosition(const QVector3D& p) override;
    void SetRotation(const QQuaternion& q) override;

    void SetRadius(float radius) { m_radius = radius; }
    float GetRadius() const { return m_radius; }

    void SetColor(QColor c);
    QColor GetColor() const;

    void SetFlags(ParticleFlags flags) { m_flags = flags; }
    void AddFlag(ParticleFlags flag) { m_flags = static_cast<ParticleFlags>(m_flags | flag); }
    void RemoveFlag(ParticleFlags flag) { m_flags = static_cast<ParticleFlags>(m_flags & ~flag); }
    bool HasFlag(ParticleFlags flag) const { return (m_flags & flag) != 0; }

};
