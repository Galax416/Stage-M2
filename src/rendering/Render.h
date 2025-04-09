#pragma once

#include <QOpenGLFunctions>

#include "BoundingBox.h"
#include "Collider.h"
#include "GeometryPrimitives.h"
#include "BVH.h"

void Render(const Line& line);
void Render(const Ray& ray);

void Render(const AABB& aabb);
void Render(const OBB& obb);

void Render(const SphereCollider& sphere);

void RenderBVH(QOpenGLShaderProgram* shader, BVHNode* node, int depth = 0);