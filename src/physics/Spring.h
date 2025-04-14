#pragma once

#include <cmath>

#include "Particle.h"

// Hooke's law
// k - spring "stiffness" constant [0 to +inf] rigid when k is high
// x - displacement of spring from equalibrium
// b - constant (coefficient) dampening
// v - realtive velocity of points of spring

class Spring
{
private:
    Particle* p1;
    Particle* p2;

    double k; // [0 to x] higher = stiff sprint, lower = loose spring
    double b; // [0 to 1] higher = more dampening, lower = less dampening
    double restingLength;

public:
    Spring(double _k, double _b, double len);
    void SetParticles(Particle* _p1, Particle* _p2);
    Particle* GetP1();
    Particle* GetP2();

    void SetConstants(double _k, double _b);
    double GetK();
    double GetB();

    void Render(QOpenGLShaderProgram* shaderProgram);
    void ApplyForce(float deltaTime);
    
private:
    QVector3D GetColorSpring();
    
};