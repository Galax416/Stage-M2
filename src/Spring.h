#pragma once

#include <cmath>

#include "Geometry.h"
#include "Particle.h"

// Hooke's law
// k - spring "tightness" constant [negative to 0] rigid at 0
// x - displacement of spring from equalibrium
// b - constant (coefficient) dampening
// v - realtive velocity of points of spring

class Spring
{
public:
    Particle* p1;
    Particle* p2;

    float k; // [-x to 0] higher = stiff sprint, lower = loose spring
    float b; // [0 to 1] higher = more dampening, lower = less dampening
    float restingLength;

    QColor color;

    Spring(float _k, float _b, float len, QColor c = Qt::white);
    void SetParticles(Particle* _p1, Particle* _p2);
    Particle* GetP1();
    Particle* GetP2();

    void SetConstants(float _k, float _b);

    void Render(QOpenGLShaderProgram* shaderProgram);
    void ApplyForce(float deltaTime);
    // void SolveConstraints(const std::vector<Rigidbody*>& constraints);

};