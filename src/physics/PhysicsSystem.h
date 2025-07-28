#pragma once

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QtConcurrent> // For concurrent processing
#include <QMutex>  
#include <QMutexLocker> // For thread safety
#include <QElapsedTimer>
#include <QDebug>
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

    // Buffer
    std::vector<std::shared_ptr<Rigidbody>> pendingBodies;
    std::vector<std::shared_ptr<Spring>> pendingSprings;
    std::vector<std::shared_ptr<Rigidbody>> pendingConstraints;
    std::vector<std::shared_ptr<TriangleCollider>> pendingTriangleColliders;

    // Bounding Volume Hierarchy for collision detection
    std::unique_ptr<BVHNode<Rigidbody>> bvhRigidbodies;
    std::unique_ptr<BVHNode<TriangleCollider>> bvhTriangleColliders;

public slots:
    void renderBVH(bool render) { m_renderBVH = render; }
    void renderCollider(bool render) { m_renderCollider = render; }

public:
    PhysicsSystem() {}

    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram, float alpha = 1.0f);

    inline void AddRigidbody(std::shared_ptr<Rigidbody> body) {
        if (!body) {
            qWarning() << "Warning: Tried to add a nullptr Rigidbody!";
            return;
        }
        QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
        pendingBodies.push_back(body);
    }
    inline void RemoveRigidbody(std::shared_ptr<Rigidbody> body) {
        if (!body) {
            qWarning() << "Warning: Tried to remove a nullptr Rigidbody!";
            return;
        }
        QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
        auto it = std::remove_if(bodies.begin(), bodies.end(), [&](const std::shared_ptr<Rigidbody>& b) { return b == body; });
        if (it != bodies.end()) {
            bodies.erase(it, bodies.end());
        }
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
    inline void ClearRigidbodys() { bodies.clear(); pendingBodies.clear(); }
    inline void ClearSprings() { springs.clear(); pendingSprings.clear(); }
    inline void ClearConstraints() { constraints.clear(); pendingConstraints.clear(); triangleColliders.clear(); pendingTriangleColliders.clear(); }
    inline void ClearBVH() { bvhRigidbodies.reset(); bvhRigidbodies = nullptr; bvhTriangleColliders.reset(); bvhTriangleColliders = nullptr; }

    QVector3D GetGravity() const { return bodies.empty() ? QVector3D(0.0f, -9.82f, 0.0f) : bodies[0]->GetGravity(); }
    void ChangeGravity(const QVector3D& g);
    void ChangeFriction(float f);

    void SetUpBVH() { bvhRigidbodies = BuildBVH(constraints); bvhTriangleColliders = BuildBVH(triangleColliders); }

    std::vector<std::shared_ptr<Rigidbody>> GetRigidBodies() { return bodies; }
    
    // QElapsedTimer m_physicsTimer;
    // qint64 m_lastStepTime = 0;
    // float m_physicsDeltaTime = 0.016f;
    // void StartTimer() { m_physicsTimer.start(); m_lastStepTime = m_physicsTimer.elapsed(); }

private: 
    bool m_renderCollider { false }; // Flag to render colliders
    bool m_renderBVH { false }; // Flag to render the BVH tree
    QMutex m_dataMutex;
};