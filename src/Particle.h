#pragma once    

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QColor>
#include <vector>

#include "Constants.h"
#include "Rigidbody.h"
#include "Model.h"
#include "Geometry.h"


class Particle : public Rigidbody
{
private:
    float m_bounce { 0.7f };
    float m_radius { 0.01f }; // default radius
    QColor m_color;
    Model* m_particleModel;

public:
    Particle();
    Particle(QVector3D pos, float r, float m, bool mov = true, QColor c = Qt::white);

    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);
    // void ApplyForces();
    void SolveConstraints(const std::vector<Rigidbody*>& constraints);

    void SetPosition(const QVector3D& p) { transform.position = p; oldPosition = p; m_particleModel->transform.position = p; }

    void SetBounce(float bounce) { m_bounce = bounce; }
    float GetBounce() { return m_bounce; }

    void SetRadius(float radius) { m_radius = radius; }
    float GetRadius() { return m_radius; }

    void SetColor(QColor c) { m_color = c; }
    QColor GetColor() { return m_color; }

};
