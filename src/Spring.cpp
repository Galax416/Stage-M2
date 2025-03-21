#include "Spring.h"

Spring::Spring(float _k, float _b, float len, QColor c) : p1(nullptr), p2(nullptr), k(_k), b(_b), restingLength(len), color(c) {}

void Spring::SetParticles(Particle* _p1, Particle* _p2) 
{
    p1 = _p1;
    p2 = _p2;
    if (restingLength <= 0.0f && p1 && p2) {
        restingLength = p1->GetPosition().distanceToPoint(p2->GetPosition());
    }
}
Particle* Spring::GetP1() { return p1; }

Particle* Spring::GetP2() { return p2; }

void Spring::SetConstants(float _k, float _b) 
{
    k = _k;
    b = _b;
}

void Spring::ApplyForce(float deltaTime) 
{
    if (!p1 || !p2) return;

    /*QVector3D delta = p2->GetPosition() - p1->GetPosition();
    float length = delta.length();
    QVector3D direction = delta.normalized();
    
    float displacement = length - restingLength;
    QVector3D force = k * displacement * direction;
    
    QVector3D velocityDiff = p2->GetVelocity() - p1->GetVelocity();
    QVector3D damping = -b * velocityDiff;
    
    p1->AddImpulse(force * deltaTime + damping * deltaTime);
    p2->AddImpulse(-force * deltaTime - damping * deltaTime);*/

    QVector3D relPos = p2->GetPosition() - p1->GetPosition();
	QVector3D relVel = p2->GetVelocity() - p1->GetVelocity();

	// Prevent underflow
	for (int i = 0; i < 3; ++i) {
		relPos[i] = (fabsf(relPos[i]) < 0.0000001f) ? 0.0f : relPos[i];
		relVel[i] = (fabsf(relVel[i]) < 0.0000001f) ? 0.0f : relVel[i];
	}

	float x = relPos.length() - restingLength;
	float v = relVel.length();

	float F = (k * x) * deltaTime + (-b * v) * deltaTime;

	QVector3D impulse = relPos.normalized() * F;
	p1->AddImpulse(impulse * p1->InvMass());
	p2->AddImpulse(impulse*  -1.0f * p2->InvMass());

}

void Spring::Render(QOpenGLShaderProgram* shaderProgram)
{
    if (!p1 || !p2) return;

    shaderProgram->setUniformValue("color", color);

    Line line(p1->GetPosition(), p2->GetPosition());
    ::Render(line);
}