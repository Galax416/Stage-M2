#pragma once

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QtConcurrent> // For concurrent processing
#include <QMutex>  
#include <QMutexLocker> // For thread safety
#include <vector>
#include <memory>

#include "CustomOBJLoader.h"
#include "Rigidbody.h"
// #include "Particle.h"
#include "Spring.h"
#include "TriangleCollider.h"
#include "BVH.h"

class PhysicsSystem : public QObject
{
    Q_OBJECT

protected:
    // 
    std::vector<std::shared_ptr<Rigidbody>> bodies;
    std::vector<std::shared_ptr<Spring>> springs;
    std::vector<std::shared_ptr<Rigidbody>> constraints;
    std::vector<std::shared_ptr<TriangleCollider>> triangleColliders;
    std::vector<Transform> rigidbodyTransformations;

    // Buffer
    std::vector<std::shared_ptr<Rigidbody>> pendingBodies;
    std::vector<std::shared_ptr<Spring>> pendingSprings;
    std::vector<std::shared_ptr<Rigidbody>> pendingConstraints;
    std::vector<std::shared_ptr<TriangleCollider>> pendingTriangleColliders;
    // std::vector<Transform> pendingRigidbodyTransformations;


    // Bounding Volume Hierarchy for collision detection
    std::unique_ptr<BVHNode<Rigidbody>> bvhRigidbodies;
    std::unique_ptr<BVHNode<TriangleCollider>> bvhTriangleColliders;

public slots:
    void renderBVH(bool render) { m_renderBVH = render; }

public:
    PhysicsSystem();

    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);

    inline void AddRigidbody(std::shared_ptr<Rigidbody> body) {
        if (!body) {
            qWarning() << "Warning: Tried to add a nullptr Rigidbody!";
            return;
        }
        QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
        pendingBodies.push_back(body);
        rigidbodyTransformations.push_back(body->transform);
    }
    inline void AddConstraint(std::shared_ptr<Rigidbody> constraint) {
        if (!constraint) {
            qWarning() << "Warning: Tried to add a nullptr Constraint!";
            return;
        }
        QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
        pendingConstraints.push_back(constraint);
    }
    inline void AddTriangleCollider(std::shared_ptr<TriangleCollider> triangle) {
        if (!triangle) {
            qWarning() << "Warning: Tried to add a nullptr TriangleCollider!";
            return;
        }
        QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
        pendingTriangleColliders.push_back(triangle);
    }
    inline void AddSpring(std::shared_ptr<Spring> spring) {
        if (!spring) {
            qWarning() << "Warning: Tried to add a nullptr Spring!";
            return;
        }
        QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
        pendingSprings.push_back(spring);
    }

    inline void ClearAll() { QMutexLocker locker(&m_dataMutex); ClearRigidbodys(); ClearSprings(); ClearConstraints(); ClearBVH(); }
    inline void ClearRigidbodys() { bodies.clear(); pendingBodies.clear(); rigidbodyTransformations.clear(); }
    inline void ClearSprings() { springs.clear(); pendingSprings.clear(); }
    inline void ClearConstraints() { constraints.clear(); pendingConstraints.clear(); triangleColliders.clear(); pendingTriangleColliders.clear(); }
    inline void ClearBVH() { bvhRigidbodies.reset(); bvhRigidbodies = nullptr; bvhTriangleColliders.reset(); bvhTriangleColliders = nullptr; }

    void ChangeGravity(const QVector3D& g);
    void ChangeFriction(float f);

    void RotateRigidbodies(QVector3D rotation);
    void SetUpBVH() { bvhRigidbodies = BuildBVH(constraints); bvhTriangleColliders = BuildBVH(triangleColliders); }

    std::vector<std::shared_ptr<Rigidbody>> GetRigidBodies() { return bodies; }

private: 
    bool m_renderBVH { false }; // Flag to render the BVH tree
    // std::vector<Transform> m_currentTransforms; // Render use
    // std::vector<Transform> m_nextTransforms; // Physics use
    QMutex m_dataMutex; 
    // QMutex m_constraintsMutex;
    // QMutex m_springsMutex;
    // QMutex m_trianglesMutex;
};