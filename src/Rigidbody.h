#pragma once

#include <QOpenGLShaderProgram>

#include "Geometry.h"
#include "Utils.h"

#define RIGIDBODY_TYPE_BASE		0
#define RIGIDBODY_TYPE_PARTICLE	1
#define RIGIDBODY_TYPE_SPHERE	2
#define RIGIDBODY_TYPE_BOX		3

class Rigidbody 
{
public:
    int type;

    QVector3D gravity { 0.0f, -9.82f, 0.0f };
    float friction { 0.99f };
    QVector3D forces { 0.0f, 0.0f, 0.0f };

    float mass { 1.0f }; // default mass
    bool movable { true };

    inline Rigidbody() {
        type = RIGIDBODY_TYPE_BASE;
    }
    virtual inline ~Rigidbody() { }

    virtual inline void Update(float deltaTime) { }
    virtual inline void Render(QOpenGLShaderProgram* shaderProgram) { }
    virtual inline void ApplyForces() { }
    virtual inline void SolveConstraints(const std::vector<Rigidbody*>& constraints) { }

    inline bool HasVolume() {
        return type == RIGIDBODY_TYPE_SPHERE || type == RIGIDBODY_TYPE_BOX;
    }

    void SetGravity(const QVector3D& g) { gravity = g; }
    void SetFriction(float f) { friction = f < 0 ? 0 : f; }
    void SetMass(float m) { mass = m < 0 ? 0 : m; }
    void SetMovable(bool m) { movable = m; }

};