#pragma once

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <vector>
#include <memory>

#include "CustomOBJLoader.h"
#include "Rigidbody.h"
#include "Particle.h"
#include "Spring.h"
#include "TriangleCollider.h"
#include "BVH.h"

class PhysicsSystem : public QObject
{
    Q_OBJECT

protected:
    std::vector<std::shared_ptr<Rigidbody>> bodies;
    std::vector<std::shared_ptr<Spring>> springs;
    std::vector<std::shared_ptr<Rigidbody>> constraints;
    std::vector<std::shared_ptr<TriangleCollider>> triangleColliders;

    std::vector<QVector3D> rigidbodyPositions;

    // Bounding Volume Hierarchy for collision detection
    std::unique_ptr<BVHNode<Rigidbody>> bvhRigidbodies;
    std::unique_ptr<BVHNode<TriangleCollider>> bvhTriangleColliders;

public slots:
    void renderBVH() { m_renderBVH = !m_renderBVH; }

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
    inline void AddTriangleCollider(std::shared_ptr<TriangleCollider> triangle) {
        if (triangle) triangleColliders.push_back(triangle);
        else qWarning() << "Warning: Tried to add a nullptr TriangleCollider!";
    }
    inline void AddSpring(std::shared_ptr<Spring> spring) {
        if (spring) springs.push_back(spring);
        else qWarning() << "Warning: Tried to add a nullptr Spring!";
    }

    inline void ClearAll() { ClearRigidbodys(); ClearSprings(); ClearConstraints(); ClearBVH(); }
    inline void ClearRigidbodys() { bodies.clear(); rigidbodyPositions.clear(); }
    inline void ClearSprings() { springs.clear(); }
    inline void ClearConstraints() { constraints.clear(); triangleColliders.clear(); }
    inline void ClearBVH() { bvhRigidbodies.reset(); bvhRigidbodies = nullptr; bvhTriangleColliders.reset(); bvhTriangleColliders = nullptr; }

    void ChangeGravity(const QVector3D& g);
    void ChangeFriction(float f);

    void RotateRigidbodies(QVector3D rotation);
    void SetUpBVH() { bvhRigidbodies = BuildBVH(constraints); bvhTriangleColliders = BuildBVH(triangleColliders); }

private: 
    bool m_renderBVH { false }; // Flag to render the BVH tree

};