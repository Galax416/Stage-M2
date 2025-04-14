#pragma once    

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QColor>
#include <vector>

#include "Rigidbody.h"

class Model; // Forward declaration of Model class

class Particle : public Rigidbody
{
private:
    float  m_radius;
    QColor m_color;
    Model* m_particleModel;

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

};
