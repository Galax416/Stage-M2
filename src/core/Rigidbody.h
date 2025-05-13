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

protected:
    int type { RIGIDBODY_TYPE_BASE }; // type of rigidbody

    float mass { 1.0f }; // default mass
    float invMass { 1.0f }; // default inverse mass
    bool isStatic { true }; // is the rigidbody static or dynamic

public:
    // Transform
    Transform transform;
    QVector3D oldPosition;

    // Forces
    QVector3D gravity     { 0.0f, -9.82f, 0.0f };
    float friction        { 0.99f }; // default friction
    float restitution     { 0.0f }; // default restitution

    // Other properties
    QVector3D velocity { 0.0f, 0.0f, 0.0f };
    QVector3D forces   { 0.0f, 0.0f, 0.0f };

    // Collision volumes
    OBB boxCollider;
    SphereCollider sphereCollider;
    
    inline Rigidbody() { id = nextId++;}
    virtual inline ~Rigidbody() { }

    virtual void Update(float dt) { }
    virtual void Render(QOpenGLShaderProgram* shaderProgram);
    
    virtual void SolveConstraints(const std::vector<std::shared_ptr<Rigidbody>>& constraints);
    virtual void SolveConstraints(const std::vector<std::shared_ptr<TriangleCollider>>& constraints);

    virtual inline void SynsCollisionVolumes() 
    { 
        if (type == RIGIDBODY_TYPE_SPHERE) {
            sphereCollider.position = transform.position;
            sphereCollider.radius = transform.scale.x();
        }
        else if (type == RIGIDBODY_TYPE_BOX) {
            boxCollider.position = transform.position;
            boxCollider.size = transform.scale;
            boxCollider.orientation = transform.GetRotationMatrix();
        }
    }

    virtual inline void ApplyForce(float dt)
    {
        if (isStatic) return;
        forces += gravity * mass;
        velocity += forces * invMass * dt;
        transform.position += velocity * dt;
    }

    virtual inline void ApplyPositionCorrection(const QVector3D& correction) { if (!isStatic) transform.position += correction; }

    // Utilities
    inline bool IsStatic() const { return isStatic; }
    inline bool IsDynamic() const { return !isStatic; }
    inline float GetMass() const { return mass; }
    inline float GetInvMass() const { return isStatic ? 0.0f : invMass; }
    inline QVector3D GetPosition() const { return transform.position; }
    inline QVector3D GetVelocity() const { return velocity; }

    // State acces
    unsigned int GetID()    const { return id; }
    int GetType()           const { return type; }

    // Configuration
    void SetMass(float m) { mass = std::max(1e-4f, m); invMass = 1.0f / m; }
    void SetStatic() { isStatic = false; invMass = 0.0f; }
    void SetDynamic() { isStatic = true; invMass = 1.0f / mass; }
    void SetFriction(float f) { friction = f < 0.0f ? 0.0f : f > 1.0f ? 1.0f : f; }
    void SetRestitution(float r) { restitution = r < 0.0f ? 0.0f : r > 1.0f ? 1.0f : r; }
    void SetGravity(const QVector3D& g) { gravity = g; }

    virtual inline void SetPosition(const QVector3D& p) {}
    virtual inline void SetRotation(const QQuaternion& q) {}

    virtual AABB GetAABB() const;


    // inline float InvMass()  { return mass == 0 ? 0 : 1.0f / mass; }
    // QMatrix4x4 InvTensor();

    // bool IsMovable()        const { return isMovable; }

    // void SetType(int t)                 { type = t; }
    // void SetFriction(float f)           { friction = f < 0 ? 0 : f; }
    // void SetMass(float m)               { mass = m < 0 ? 0 : m; }
    // void SetCor(float c)                { cor = c < 0 ? 0 : c > 1 ? 1 : c; }
    // void SetMovable(bool m)             { isMovable = m; }
};