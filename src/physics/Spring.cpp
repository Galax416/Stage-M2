#include "Spring.h"
#include "Render.h"
#include "Utils.h"

Spring::Spring(float _k) : p1(nullptr), p2(nullptr), k(_k), b(0.0f), restingLength(0.0f) {}

Spring::Spring(float _k, float _b, float len) : p1(nullptr), p2(nullptr), k(_k), b(_b), restingLength(len) 
{
    b = std::clamp(b, 0.0f, 1.0f); // Clamp damping coefficient
    restingLength = std::max(0.0f, restingLength); // Prevent negative length
}

void Spring::SetParticles(std::shared_ptr<Particle> _p1, std::shared_ptr<Particle> _p2) 
{
    p1 = _p1;
    p2 = _p2;
    
    if (restingLength == 0.0f && p1 && p2) restingLength = (p2->GetPosition() - p1->GetPosition()).length();
    if (b == 0.0f) b = 2 * sqrt(k * (p1->GetMass() * p2->GetMass()) / (p1->GetMass() + p2->GetMass())); // Critical damping
    
    m_color = floatToQColor(k); // Set color based on spring constant

}
std::shared_ptr<Particle> Spring::GetP1() { return p1; }

std::shared_ptr<Particle> Spring::GetP2() { return p2; }

void Spring::SetConstants(float _k, float _b) 
{
    k = _k;
    b = _b;
}

float Spring::GetK() { return k; }

float Spring::GetB() { return b; }

void Spring::ApplyForce(float deltaTime) 
{
    if (!p1 || !p2) return;

    
    QVector3D relPos = p2->GetPosition() - p1->GetPosition();
    QVector3D relVel = p2->GetVelocity() - p1->GetVelocity();
    
    // Prevent underflow
	for (int i = 0; i < 3; ++i) {
        relPos[i] = (fabsf(relPos[i]) < 1e-6f) ? 0.0f : relPos[i];
		relVel[i] = (fabsf(relVel[i]) < 1e-6f) ? 0.0f : relVel[i];
	}
    
    float length = relPos.length();
    if (length < 1e-6f) return; // Avoid division by zero
    
    // if (IsRigid()) {
    //     float diff = (length - restingLength) / length;
    //     QVector3D correction = relPos * 0.5f * diff;

    //     if (p1->IsMovable()) p1->transform.position += correction;
    //     if (p2->IsMovable()) p2->transform.position -= correction;
    //     return;
    // }

    float w1 = p1->IsMovable() ? p1->InvMass() : 0.0f;
    float w2 = p2->IsMovable() ? p2->InvMass() : 0.0f;
    float w = w1 + w2;
    if (w == 0.0f) return; // Both particles are immovable

    float diff = length - restingLength;

    QVector3D direction = relPos.normalized();

    float x = diff;
    float v = QVector3D::dotProduct(relVel, direction);

    if (std::isnan(x)) x = 0.0f; // Prevent NaN from causing issues
    if (std::fabs(x) < 1e-6f) x = 0.0f; // Prevent underflow
    if (std::fabs(x) > 1e6f) x = 0.0f; // Prevent overflow
    if (std::isnan(v)) v = 0.0f; // Prevent NaN from causing issues
    if (std::fabs(v) < 1e-6f) v = 0.0f; // Prevent underflow
    if (std::fabs(v) > 1e6f) v = 0.0f; // Prevent overflow

    float F = (-k * x) + (-b * v); // Hooke's law

    QVector3D impulse = direction * F * deltaTime;

    if (p1->IsMovable()) p1->AddLinearImpulse(-impulse * w1);
    if (p2->IsMovable()) p2->AddLinearImpulse( impulse * w2);

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