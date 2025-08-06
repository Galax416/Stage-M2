#pragma once

#include <QVector3D>
#include "GeometryPrimitives.h"
#include "Geometry3D.h"
#include "Mesh.h"

#include <cmath>
#include <memory>

// Forward declarations
class OBB;
class SphereCollider;
class TriangleCollider;

RayCastResult RayIntersectsSphere(const Ray& ray, const SphereCollider& sphere);
RayCastResult RayIntersectsOBB(const Ray& ray, const OBB& obb);
RayCastResult RayIntersectsTriangle(const Ray& ray, const std::shared_ptr<TriangleCollider>& tri);
RayCastResult RayIntersectsTriangle(const Ray& ray, const Triangle& tri);

// bool RayIntersectsTriangle(const Ray& ray, const std::shared_ptr<TriangleCollider>& tri);