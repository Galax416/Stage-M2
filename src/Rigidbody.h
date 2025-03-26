#pragma once

#include <QOpenGLShaderProgram>

#include "Transform.h"
#include "Collision.h"
// #include "Geometry.h"

#define RIGIDBODY_TYPE_BASE		0
#define RIGIDBODY_TYPE_PARTICLE	1
#define RIGIDBODY_TYPE_SPHERE	2
#define RIGIDBODY_TYPE_BOX		3

class Rigidbody
{
public:
    int type; // 0: base, 1: particle, 2: sphere, 3: box

    // Transform
    Transform transform;

    // Physics properties
    QVector3D velocity { 0.0f, 0.0f, 0.0f };

    QVector3D orientation { 0.0f, 0.0f, 0.0f };
    QVector3D angVel { 0.0f, 0.0f, 0.0f };

    QVector3D forces { 0.0f, 0.0f, 0.0f };
    QVector3D torques { 0.0f, 0.0f, 0.0f };

    QVector3D gravity { 0.0f, -9.82f, 0.0f };

    float mass { 1.0f }; // default mass
    float cor { 0.5f }; // Coefficient of restitution

    float friction { 0.99f }; // default friction

    bool isMovable { true }; // is movable

    // Collision volumes
    OBB box;
    // Sphere sphere;

    inline Rigidbody() {
        type = RIGIDBODY_TYPE_BASE;
    }
    virtual inline ~Rigidbody() { }

    virtual inline void Update(float deltaTime) { }
    virtual inline void Render(QOpenGLShaderProgram* shaderProgram) { }
    virtual inline void ApplyForces() { }
    virtual inline void SolveConstraints(const QVector<Rigidbody*>& constraints) { }

    inline bool HasVolume() { return type == RIGIDBODY_TYPE_SPHERE || type == RIGIDBODY_TYPE_BOX; }
    inline float InvMass() { return mass == 0 ? 0 : 1.0f / mass; }

    void SetGravity(const QVector3D& g) { gravity = g; }
    void SetFriction(float f) { friction = f < 0 ? 0 : f; }
    void SetMass(float m) { mass = m < 0 ? 0 : m; }
    void SetMovable(bool m) { isMovable = m; }

};