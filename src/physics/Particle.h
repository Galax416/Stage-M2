#pragma once    

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QColor>
#include <vector>

#include "Rigidbody.h"

enum ParticleFlags
{
    PARTICLE_FREE = 0,
    PARTICLE_ATTACHED_TO_TRIANGLE = 1 << 0,
    PARTICLE_NO_COLLISION_WITH_US = 1 << 1
};

class Model; // Forward declaration of Model class

class Particle : public Rigidbody
{
private:
    float  m_radius;
    QColor m_color;
    Model* m_particleModel;
    ParticleFlags m_flags;

public:
    // Particle();
    Particle(QVector3D pos, float r, float m, bool mov = true, QColor color = Qt::white);

    void Update(float deltaTime) override;
    void Render(QOpenGLShaderProgram* shaderProgram) override;

    void SetPosition(const QVector3D& p) override;

    void SetRadius(float radius) { m_radius = radius; }
    float GetRadius() { return m_radius; }

    void SetColor(QColor c);
    QColor GetColor() { return m_color; }

    void SetFlags(ParticleFlags flags) { m_flags = flags; }
    ParticleFlags GetFlags() { return m_flags; }

};
