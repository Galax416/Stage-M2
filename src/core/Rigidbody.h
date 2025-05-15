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

// Forward declaration
class Particle;
struct TriangleCollider;

class Rigidbody
{
private:
    unsigned int id;
    static unsigned int nextId;

public:
    int type; // 0: base, 1: particle, 2: sphere, 3: box

    // Transform
    Transform transform;
    QVector3D oldPosition;

    QVector3D orientation { 0.0f, 0.0f, 0.0f };
    QVector3D angVel      { 0.0f, 0.0f, 0.0f };
    QVector3D forces      { 0.0f, 0.0f, 0.0f };
    QVector3D torques     { 0.0f, 0.0f, 0.0f };
    QVector3D gravity     { 0.0f, -9.82f, 0.0f };
    float mass            { 1.0f }; // default mass
    float cor             { 0.05f }; // Coefficient of restitution
    float friction        { 0.99f }; // default friction
    bool isMovable        { true }; // is movable

    // Collision volumes
    OBB boxCollider;
    SphereCollider sphereCollider;
    
    inline Rigidbody() {
        id = nextId++;
        type = RIGIDBODY_TYPE_BASE;
    }
    virtual inline ~Rigidbody() { }

    virtual inline void Update(float deltaTime) { }
    virtual void Render(QOpenGLShaderProgram* shaderProgram);
    
    virtual inline void ApplyForces() { forces = gravity * mass; } // gravity
    virtual void AddLinearImpulse(const QVector3D& impulse);
    virtual void AddRotationalImpulse(const QVector3D& impulse) {}
    virtual void SolveConstraints(const std::vector<std::shared_ptr<Rigidbody>>& constraints);
    virtual void SolveConstraints(const std::vector<std::shared_ptr<TriangleCollider>>& constraints);

    virtual inline void SynsCollisionVolumes() { }

    // inline bool HasVolume() { return type == RIGIDBODY_TYPE_SPHERE || type == RIGIDBODY_TYPE_BOX; }
    inline float InvMass()  { return mass == 0 ? 0 : 1.0f / mass; }
    QMatrix4x4 InvTensor();

    unsigned int GetID()    const { return id; }
    int GetType()           const { return type; }
    float GetMass()         const { return mass; }
    float GetCor()          const { return cor; }
    bool IsMovable()        const { return isMovable; }
    QVector3D GetPosition() const { return transform.position; }
    QVector3D GetVelocity() const { return transform.position - oldPosition; }
    virtual AABB GetAABB()  const;

    void SetType(int t)                 { type = t; }
    void SetGravity(const QVector3D& g) { gravity = g; }
    void SetFriction(float f)           { friction = f < 0 ? 0 : f; }
    void SetMass(float m)               { mass = m < 0 ? 0 : m; }
    void SetCor(float c)                { cor = c < 0 ? 0 : c > 1 ? 1 : c; }
    void SetMovable(bool m)             { isMovable = m; }
    virtual inline void SetPosition(const QVector3D& p) {}
    virtual inline void SetRotation(const QQuaternion& q) {}

};