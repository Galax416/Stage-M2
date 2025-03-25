#pragma once    

#include <QVector3D>
#include <QColor>
#include <QOpenGLShaderProgram>

#include "Rigidbody.h"

class Particle : public Rigidbody
{
private:
    QVector3D position { 0.0f, 0.0f, 0.0f };
    QVector3D oldPosition { 0.0f, 0.0f, 0.0f };   

    float bounce { 0.7f };

    float radius { 0.01f }; // default radius
    QColor color;

public:
    Particle();
    Particle(QVector3D pos, float r, float m, bool mov = true, QColor c = Qt::white);

    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);
    void ApplyForces();
    void SolveConstraints(const std::vector<Rigidbody*>& constraints);

    void SetPosition(const QVector3D& pos) { position = pos; oldPosition = position; }
    QVector3D GetPosition() { return position; }

    void SetBounce(float b) { bounce = b; }
    float GetBounce() { return bounce; }

    void SetRadius(float r) { radius = r; }
    float GetRadius() { return radius; }

    void AddImpulse(const QVector3D& impulse);
    
    QVector3D GetVelocity() { return position - oldPosition; }
    float InvMass() { return mass == 0 ? 0 : 1.0f / mass; }
    void SetMass(float m) { mass = m < 0 ? 0 : m; }

};
