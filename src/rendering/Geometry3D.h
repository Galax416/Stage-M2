#pragma once 

#include "Model.h"
#include <memory>

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
    static std::shared_ptr<Mesh> GetSharedPlaneMesh();

    QVector3D normal;
    float distance;
};

class Box : public Model
{
public:
    Box();
    Box(const QVector3D& p, const QVector3D& s);
    Box(const QVector3D& p, const QVector3D& s, QColor c);

private:
    void SetUpBox();
    static std::shared_ptr<Mesh> GetSharedBoxMesh();
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
    static std::shared_ptr<Mesh> GetSharedSphereMesh();

    float radius;
};