#include "Spring.h"

Spring::Spring(float _k, float _b, float len, QColor c) : p1(nullptr), p2(nullptr), k(_k), b(_b), restingLength(len), color(c) {}

void Spring::SetParticles(Particle* _p1, Particle* _p2) 
{
    p1 = _p1;
    p2 = _p2;
    if (restingLength <= 0.0f && p1 && p2) {
        restingLength = p1->transform.position.distanceToPoint(p2->transform.position);
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

    QVector3D relPos = p2->transform.position - p1->transform.position;
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
	p1->AddLinearImpulse(impulse * p1->InvMass());
	p2->AddLinearImpulse(impulse*  -1.0f * p2->InvMass());

}

void Spring::Render(QOpenGLShaderProgram* shaderProgram)
{
    if (!p1 || !p2) return;

    shaderProgram->bind();

    shaderProgram->setUniformValue("color", color);
    shaderProgram->setUniformValue("model", QMatrix4x4());

    Line line(p1->transform.position, p2->transform.position);
    ::Render(line);

    shaderProgram->release();

}