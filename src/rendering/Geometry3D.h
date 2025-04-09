#pragma once 

#include "Model.h"

class Plane : public Model
{
public:
    Plane();
    Plane(const QVector3D& p, const QVector3D& n);
    Plane(const QVector3D& p, const QVector3D& n, QColor c);

    QVector3D GetNormal() const { return normal; }
    void SetNormal(const QVector3D& n) { normal = n; }

    float GetDistance() const { return distance; }
    void SetDistance(float d) { distance = d; }

private:
    void SetUpPlane(const QVector3D& n = QVector3D(0.0, 1.0, 0.0));
    QVector3D normal;
    float distance;
};


class Sphere : public Model
{
public:
    Sphere();
    Sphere(const QVector3D& p, float r);
    Sphere(const QVector3D& p, float r, QColor c);

    float GetRadius() const { return radius; }
    void SetRadius(float r) { radius = r; }

private:
    void SetUpSphere();
    float radius;
};