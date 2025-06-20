#include "Spring.h"
#include "Particle.h"
#include "Render.h"
#include "Utils.h"

Spring::Spring(std::shared_ptr<Particle> a, std::shared_ptr<Particle> b, float k)
    : p1(a), p2(b)
{
    if (!p1 && !p2) return;
    restingLength = (p2->GetPosition() - p1->GetPosition()).length();
    SetStiffness(k);

    m_color = floatToQColor(stiffness); // Set color based on spring constant
}

void Spring::Render(QOpenGLShaderProgram* shaderProgram)
{
    if (!p1 || !p2) return;

    shaderProgram->bind();

    shaderProgram->setUniformValue("material.albedo", QVector3D(m_color.redF(), m_color.greenF(), m_color.blueF()));
    shaderProgram->setUniformValue("transparency", static_cast<GLfloat>(m_color.alphaF()));
    shaderProgram->setUniformValue("model", QMatrix4x4());

    Line line(p1->GetPosition(), p2->GetPosition());
    ::Render(line);

    shaderProgram->release();

}

void Spring::SolveConstraints(float dt)
{
    if (!p1 || !p2) return;

    QVector3D x1 = p1->GetPosition();
    QVector3D x2 = p2->GetPosition();
    QVector3D delta = x2 - x1;
    
    float length = delta.length();
    if (length == 0.0f) return; // Prevent division by zero

    float w1 = p1->GetInvMass();
    float w2 = p2->GetInvMass();
    float wSum = w1 + w2;
    if (wSum == 0.0f) return; // Prevent division by zero

    float C = length - restingLength;
    float denom = wSum + compliance / (dt * dt);
    if (denom < 1e-6f) return; // Prevent division by zero

    float dlambda = (-C - compliance * lambda) / denom;
    lambda += dlambda;

    QVector3D correction = delta.normalized() * dlambda;

    if (p1->IsDynamic()) p1->ApplyPositionCorrection(-correction * w1);
    if (p2->IsDynamic()) p2->ApplyPositionCorrection( correction * w2);
}

void Spring::SetStiffness(float k)
{
    stiffness = k < 1.0f ? 1.0f : k; 
    float t = (stiffness - 1.0f) / (1000.0f - 1.0f); // ∈ [0, 1]
    compliance = std::exp(std::log(0.999999f) * (1.0f - t) + std::log(1e-6f) * t);
    // float scale = std::pow(stiffness / 1000.0f, 6); 
    // scale = std::clamp(scale, 1e-6f, 0.999999f); // Clamp to avoid extreme values
    // qDebug() << "Compliance:" << 1.0f / stiffness << "scale:" << scale << "compliance scaled:" << 1.0f - scale;
    // compliance = 1.0f / stiffness; 
}