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

};