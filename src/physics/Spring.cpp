#include "Spring.h"
#include "Render.h"
#include "Utils.h"

Spring::Spring(double _k) : p1(nullptr), p2(nullptr), k(_k), b(0.0f), restingLength(0.0f) {}

Spring::Spring(double _k, double _b, double len) : p1(nullptr), p2(nullptr), k(_k), b(_b), restingLength(len) {}

void Spring::SetParticles(std::shared_ptr<Particle> _p1, std::shared_ptr<Particle> _p2) 
{
    p1 = _p1;
    p2 = _p2;
    if (restingLength <= 0.0f && p1 && p2) restingLength = p1->transform.position.distanceToPoint(p2->transform.position);
    b = 2 * sqrt(k * (p1->GetMass() * p2->GetMass()) / (p1->GetMass() + p2->GetMass())); // Critical damping
    m_color = floatToQColor(k); // Set color based on spring constant
    // m_usePBDCorrection = false; // Default to no PBD correction
    // m_rigidity = false; // Default to not rigid
    // m_initialRelPos = p2->transform.position - p1->transform.position;
}
std::shared_ptr<Particle> Spring::GetP1() { return p1; }

std::shared_ptr<Particle> Spring::GetP2() { return p2; }

void Spring::SetConstants(double _k, double _b) 
{
    k = _k;
    b = _b;
}

double Spring::GetK() { return k; }

double Spring::GetB() { return b; }

void Spring::ApplyForce(float deltaTime) 
{
    if (!p1 || !p2) return;

    QVector3D relPos = p2->transform.position - p1->transform.position;
    double length = relPos.length();
    if (length < 1e-6f) return; // Avoid division by zero

    float w1 = p1->IsMovable() ? p1->InvMass() : 0.0f;
    float w2 = p2->IsMovable() ? p2->InvMass() : 0.0f;
    float w = w1 + w2;
    if (w == 0.0f) return; // Both particles are immovable

    float diff = length - restingLength;

    QVector3D relVel = p2->GetVelocity() - p1->GetVelocity();

    QVector3D direction = relPos.normalized();

    double x = diff;
    double v = QVector3D::dotProduct(relVel, direction);

    double F = (-k * x) + (-b * v); // Hooke's law

    QVector3D impulse = direction * F * deltaTime;

    if (p1->IsMovable()) p1->AddLinearImpulse(-impulse * w1);
    if (p2->IsMovable()) p2->AddLinearImpulse( impulse * w2);

}

void Spring::Render(QOpenGLShaderProgram* shaderProgram)
{
    if (!p1 || !p2) return;

    shaderProgram->bind();

    shaderProgram->setUniformValue("material.albedo", QVector3D(m_color.redF(), m_color.greenF(), m_color.blueF()));
    shaderProgram->setUniformValue("model", QMatrix4x4());

    Line line(p1->transform.position, p2->transform.position);
    ::Render(line);

    shaderProgram->release();

}