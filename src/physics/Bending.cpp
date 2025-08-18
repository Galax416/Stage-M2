#include "Bending.h"

Bending::Bending(std::shared_ptr<Particle> a, std::shared_ptr<Particle> b, 
                 std::shared_ptr<Particle> c, std::shared_ptr<Particle> d, 
                 float k)
    : p1(a), p2(b), p3(c), p4(d), compliance(k), lambda(0.0f)
{
    restingAngle = ComputeDihedralAngle();
}

float Bending::ComputeDihedralAngle() const
{
    QVector3D x1 = p1->GetPosition();
    QVector3D x2 = p2->GetPosition();
    QVector3D x3 = p3->GetPosition();
    QVector3D x4 = p4->GetPosition();

    QVector3D n1 = QVector3D::crossProduct(x2 - x1, x3 - x1).normalized();
    QVector3D n2 = QVector3D::crossProduct(x1 - x2, x4 - x2).normalized();
    float dot = QVector3D::dotProduct(n1, n2);
    float angle = std::acos(dot < -1.0f ? -1.0f : (dot > 1.0f ? 1.0f : dot));
    // float sign = QVector3D::dotProduct(e, QVector3D::crossProduct(n1, n2)) < 0.0f ? -1.0f : 1.0f;
    // angle *= sign;
    return angle;
}

void Bending::SolveConstraints(float dt)
{
    if (!p1 || !p2 || !p3 || !p4) return;

    float angle = ComputeDihedralAngle();

    float C = angle - restingAngle;

    float w1 = p1->GetInvMass();
    float w2 = p2->GetInvMass();
    float w3 = p3->GetInvMass();
    float w4 = p4->GetInvMass();
    float wSum = w1 + w2 + w3 + w4;
    if (wSum == 0.0f) return; // Avoid division by zero

    float denom = wSum + compliance / (dt * dt);
    float dlambda = (-C - compliance * lambda) / denom;
    lambda += dlambda;

    // Compute n1 and n2 as in ComputeDihedralAngle
    QVector3D x1 = p1->GetPosition();
    QVector3D x2 = p2->GetPosition();
    QVector3D x3 = p3->GetPosition();
    QVector3D x4 = p4->GetPosition();
    QVector3D n1 = QVector3D::crossProduct(x2 - x1, x3 - x1).normalized();
    QVector3D n2 = QVector3D::crossProduct(x1 - x2, x4 - x2).normalized();

    QVector3D correction = dlambda * QVector3D::crossProduct(n1, n2).normalized();

    if (p1->IsDynamic()) p1->ApplyPositionCorrection( correction * w1);
    if (p2->IsDynamic()) p2->ApplyPositionCorrection( correction * w2);
    if (p3->IsDynamic()) p3->ApplyPositionCorrection(-correction * w3);
    if (p4->IsDynamic()) p4->ApplyPositionCorrection(-correction * w4);
}