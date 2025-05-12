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
    std::shared_ptr<Particle> p1;
    std::shared_ptr<Particle> p2;

    float k; // [0 to x] higher = stiff sprint, lower = loose spring
    float b; // [0 to 1] higher = more dampening, lower = less dampening
    float restingLength;

    QColor m_color;
    // bool m_rigidity { false };
    // bool m_usePBDCorrection { false };

public:
    Spring(float _k);
    Spring(float _k, float _b, float len);
    void SetParticles(std::shared_ptr<Particle> _p1, std::shared_ptr<Particle> _p2);
    std::shared_ptr<Particle> GetP1();
    std::shared_ptr<Particle> GetP2();

    void SetConstants(float _k, float _b);
    float GetK();
    float GetB();
    float GetRestLength() { return restingLength; }

    void Render(QOpenGLShaderProgram* shaderProgram);
    void ApplyForce(float deltaTime);

    void SetColor(QColor c) { m_color = c; }
    QColor GetColor() { return m_color; }

    // void SetRigidity(bool r) { m_rigidity = r; }
    // bool IsRigid() const { return m_rigidity; }
    // void EnablePBD(bool enable) { m_usePBDCorrection = enable; }
    // bool IsPBDEnabled() const { return m_usePBDCorrection; }
    
};