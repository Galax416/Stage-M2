#pragma once

#include <QOpenGLShaderProgram>

#include "Rigidbody.h"
#include "Spring.h"

#include <vector>

class PhysicsSystem 
{
protected:
    std::vector<Rigidbody*> bodies;
    std::vector<Spring> springs;
    std::vector<Rigidbody*> constraints;

public:
    PhysicsSystem();

    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);

    void AddRigidbody(Rigidbody* body) { bodies.push_back(body); }
    void AddSpring(Spring& spring) { springs.push_back(spring); }
    void AddConstraint(Rigidbody* constraint) { constraints.push_back(constraint); }

    void ClearAll() { ClearRigidbodys(); ClearSprings(); ClearConstraints(); }
    void ClearRigidbodys() { bodies.clear(); }
    void ClearSprings() { springs.clear(); }
    void ClearConstraints() { constraints.clear(); }

    void ChangeGravityParticle(const QVector3D& g);
    void ChangeFrictionParticle(float f);

};