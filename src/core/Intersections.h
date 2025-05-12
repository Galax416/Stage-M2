#pragma once

#include <QVector3D>
#include "GeometryPrimitives.h"

#include <cmath>
#include <memory>

// Forward declarations
class TriangleCollider;

bool RayIntersectsTriangle(const Ray& ray, const std::shared_ptr<TriangleCollider>& tri);