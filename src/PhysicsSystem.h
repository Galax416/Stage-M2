#pragma once

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <vector>
#include <memory>

#include "CustomOBJLoader.h"
#include "Rigidbody.h"
#include "Particle.h"
#include "Spring.h"

class PhysicsSystem 
{
protected:
    std::vector<Rigidbody*> bodies;
    std::vector<Spring> springs;
    std::vector<Rigidbody*> constraints;

    std::vector<QVector3D> rigidbodyPositions;

public:
    PhysicsSystem();

    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);

    void AddRigidbody(Rigidbody* body) { bodies.push_back(body); rigidbodyPositions.push_back(body->transform.position); }
    void AddSpring(Spring& spring) { springs.push_back(spring); }
    void AddConstraint(Rigidbody* constraint) { constraints.push_back(constraint); }

    void ClearAll() { ClearRigidbodys(); ClearSprings(); ClearConstraints(); }
    void ClearRigidbodys() { bodies.clear(); }
    void ClearSprings() { springs.clear(); }
    void ClearConstraints() { constraints.clear(); }

    void ChangeGravity(const QVector3D& g);
    void ChangeFriction(float f);

    void RotateRigidbodies(QVector3D rotation);

};

void ConvertModelToParticleSprings(Model* model, std::vector<std::unique_ptr<Particle>> &particles, std::vector<std::unique_ptr<Spring>> &springs);
void ChargeModelParticleSprings(Model* model, std::vector<std::unique_ptr<Particle>> &particles, std::vector<std::unique_ptr<Spring>> &springs);