#pragma once

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <vector>
#include <memory>

#include "CustomOBJLoader.h"
#include "Rigidbody.h"
#include "Particle.h"
#include "Spring.h"
#include "BVH.h"

class PhysicsSystem 
{
protected:
    std::vector<std::shared_ptr<Rigidbody>> bodies;
    std::vector<Spring> springs;
    std::vector<std::shared_ptr<Rigidbody>> constraints;

    std::vector<QVector3D> rigidbodyPositions;

    std::unique_ptr<BVHNode> bvhRoot; // Bounding Volume Hierarchy for collision detection

public:
    PhysicsSystem();

    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);

    inline void AddRigidbody(std::shared_ptr<Rigidbody> body) {
        if (body) {
            bodies.push_back(body);
            rigidbodyPositions.push_back(body->transform.position);
        }
        else qWarning() << "Warning: Tried to add a nullptr Rigidbody!";
    }
    inline void AddConstraint(std::shared_ptr<Rigidbody> constraint) {
        if (constraint) constraints.push_back(constraint);
        else qWarning() << "Warning: Tried to add a nullptr Constraint!";
    }
    inline void AddSpring(Spring& spring) { springs.push_back(spring); }

    inline void ClearAll() { ClearRigidbodys(); ClearSprings(); ClearConstraints(); ClearBVH(); }
    inline void ClearRigidbodys() { bodies.clear(); rigidbodyPositions.clear(); }
    inline void ClearSprings() { springs.clear(); }
    inline void ClearConstraints() { constraints.clear(); }
    inline void ClearBVH() { bvhRoot.reset(); bvhRoot = nullptr; }

    void ChangeGravity(const QVector3D& g);
    void ChangeFriction(float f);

    void RotateRigidbodies(QVector3D rotation);

};