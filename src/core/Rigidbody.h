#pragma once

#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QColor>
#include <vector>

#include "Transform.h"
#include "BoundingBox.h"
#include "SphereCollider.h"

#define RIGIDBODY_TYPE_BASE		0
#define RIGIDBODY_TYPE_PARTICLE	1
#define RIGIDBODY_TYPE_SPHERE	2
#define RIGIDBODY_TYPE_BOX		3
#define RIGIDBODY_TYPE_TRIANGLE	4

// Forward declaration
class Particle;
struct TriangleCollider;

class Rigidbody
{
private:
    unsigned int id;
    static unsigned int nextId;

protected:
    int type { RIGIDBODY_TYPE_BASE }; // type of rigidbody

    float mass { 1.0f }; // default mass
    float invMass { 1.0f }; // default inverse mass
    bool isStatic { true }; // is the rigidbody static or dynamic
    // bool isDirty { false }; // is the rigidbody dirty (needs to be updated)

public:
    // Transform
    Transform transform;
    QVector3D oldPosition;
    // QVector3D currentPosition;
    // QVector3D previousPosition;
    // QVector3D displayPosition; // for rendering purposes

    // Forces
    QVector3D gravity     { 0.0f, -9.82f, 0.0f };
    float friction        { 0.99f }; // default friction
    float restitution     { 0.0f }; // default restitution

    // Other properties
    // QVector3D velocity { 0.0f, 0.0f, 0.0f };
    QVector3D forces   { 0.0f, 0.0f, 0.0f };
    // long unsigned int m_sleepCount { 0 }; // Counter for sleeping rigidbodies

    // Collision volumes
    OBB boxCollider;
    SphereCollider sphereCollider;
    std::vector<std::shared_ptr<TriangleCollider>> triangleColliders;
    
    inline Rigidbody() { id = nextId++;}
    virtual inline ~Rigidbody() { }

    virtual void Update(float dt) { }
    virtual void Render(QOpenGLShaderProgram* shaderProgram);
    
    virtual void SolveConstraints(const std::vector<std::shared_ptr<Rigidbody>>& constraints);
    virtual void SolveConstraints(const std::vector<std::shared_ptr<TriangleCollider>>& constraints);

    virtual inline void SyncCollisionVolumes() { }

    virtual void ApplyPositionCorrection(const QVector3D& correction);
    virtual void ApplyRotationCorrection(const QVector3D& torque);

    // Utilities
    inline bool IsStatic()         const { return isStatic; }
    inline bool IsDynamic()        const { return !isStatic; }
    // inline bool IsDirty()          const { return isDirty; }
    inline float GetMass()         const { return mass; }
    inline float GetInvMass()      const { return isStatic ? 0.0f : invMass; }
    inline float GetFriction()     const { return friction; }
    inline QVector3D GetGravity()  const { return gravity; }
    inline QVector3D GetPosition() const { return transform.position; }
    // inline QVector3D GetVelocity() const { return velocity; }

    // State acces
    unsigned int GetID()    const { return id; }
    int GetType()           const { return type; }

    // Configuration
    void SetMass(float m)               { mass = m; m <= 0.0f ? invMass = 0.0f : invMass = 1.0f / m; }
    void SetStatic()                    { isStatic = true; }
    void SetDynamic()                   { isStatic = false; }
    void SetFriction(float f)           { friction = f < 0.0f ? 0.0f : f > 1.0f ? 1.0f : f; }
    void SetRestitution(float r)        { restitution = r < 0.0f ? 0.0f : r > 1.0f ? 1.0f : r; }
    void SetGravity(const QVector3D& g) { gravity = g; }
    // void SetDirtyFlag(bool dirty)       { isDirty = dirty; }
    // void SetVelocity(const QVector3D& v) { velocity = v; }

    virtual inline void SetPosition(const QVector3D& p)   { }
    virtual inline void SetRotation(const QQuaternion& q) { }

    virtual AABB GetAABB() const;
};