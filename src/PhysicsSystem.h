#pragma once

#include <QOpenGLShaderProgram>
#include <QVector>

#include "Rigidbody.h"
#include "Spring.h"

class PhysicsSystem 
{
protected:
    QVector<Rigidbody*> bodies;
    QVector<Spring> springs;
    QVector<Rigidbody*> constraints;

public:
    PhysicsSystem();

    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);

    void AddRigidbody(Rigidbody* body) { bodies.append(body); }
    void AddSpring(Spring& spring) { springs.append(spring); }
    void AddConstraint(Rigidbody* constraint) { constraints.append(constraint); }

    void ClearAll() { ClearRigidbodys(); ClearSprings(); ClearConstraints(); }
    void ClearRigidbodys() { bodies.clear(); }
    void ClearSprings() { springs.clear(); }
    void ClearConstraints() { constraints.clear(); }

    void ChangeGravityParticle(const QVector3D& g);
    void ChangeFrictionParticle(float f);

};