#include "Spring.h"
#include "Render.h"

Spring::Spring(double _k, double _b, double len) : p1(nullptr), p2(nullptr), k(_k), b(_b), restingLength(len) {}

void Spring::SetParticles(Particle* _p1, Particle* _p2) 
{
    p1 = _p1;
    p2 = _p2;
    if (restingLength <= 0.0f && p1 && p2) restingLength = p1->transform.position.distanceToPoint(p2->transform.position);
    b = 2 * sqrt(k * (p1->GetMass() * p2->GetMass()) / (p1->GetMass() + p2->GetMass())); // Critical damping
}
Particle* Spring::GetP1() { return p1; }

Particle* Spring::GetP2() { return p2; }

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
	QVector3D relVel = p2->GetVelocity() - p1->GetVelocity();

    double length = relPos.length();
    if (length < 1e-6f) return; // Avoid division by zero

    QVector3D direction = relPos.normalized();

	double x = length - restingLength;
	double v = QVector3D::dotProduct(relVel, direction);

	double F = (-k * x) + (-b * v); // Hooke's law

	QVector3D impulse = direction * F * deltaTime;
    
	if (p1->IsMovable()) p1->AddLinearImpulse(-impulse * p1->InvMass());
	if (p2->IsMovable()) p2->AddLinearImpulse( impulse * p2->InvMass()); 

}

QVector3D Spring::GetColorSpring()
{
    QColor color1 = p1->GetColor();
    QColor color2 = p2->GetColor();

    QVector3D colorSpring = QVector3D(
        (color1.redF() + color2.redF()) * 0.5f,
        (color1.greenF() + color2.greenF()) * 0.5f,
        (color1.blueF() + color2.blueF()) * 0.5f
    );

    return colorSpring;
}

void Spring::Render(QOpenGLShaderProgram* shaderProgram)
{
    if (!p1 || !p2) return;

    shaderProgram->bind();

    shaderProgram->setUniformValue("material.albedo", GetColorSpring());
    shaderProgram->setUniformValue("model", QMatrix4x4());

    Line line(p1->transform.position, p2->transform.position);
    ::Render(line);

    shaderProgram->release();

}