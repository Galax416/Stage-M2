#pragma once

#include <cmath>

#include "Particle.h"

class Spring
{
private:
    std::shared_ptr<Particle> p1;
    std::shared_ptr<Particle> p2;

    float restingLength { 0.0f};
    float stiffness { 0.0f };
    double compliance { 1.0f };
    double lambda { 0.0f };
    
    QColor m_color;

public:
    Spring(std::shared_ptr<Particle> a, std::shared_ptr<Particle> b, float k = 1.0f);
    
    void Render(QOpenGLShaderProgram* shaderProgram);
    void SolveConstraints(float dt);

    void ResetLambda() { lambda = 0.0f; }

    std::shared_ptr<Particle> GetP1() { return p1; }
    std::shared_ptr<Particle> GetP2() { return p2; }

    float GetRestLength() { return restingLength; }
    float GetStiffness() { return stiffness; }
    float GetCompliance() { return compliance; }
    QColor GetColor() { return m_color; }

    void SetStiffness(float k);
    void SetRestLength(float len) { restingLength = len; }
    void SetCompliance(float c) { compliance = c; }
    void SetColor(QColor c) { m_color = c; }

    
};