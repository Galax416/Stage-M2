#pragma once

#include <QOpenGLFunctions>

#include "BoundingBox.h"
#include "SphereCollider.h"
#include "GeometryPrimitives.h"
#include "BVH.h"

void Render(const Line& line);
void Render(const Ray& ray);

void Render(const AABB& aabb);
void Render(const OBB& obb);

void Render(const SphereCollider& sphere);

template <typename T>
void RenderBVH(QOpenGLShaderProgram* shader, BVHNode<T>* node, int depth = 0)
{
    if (!node) return;

    static const QColor levelColors[] = {
        QColor(255, 0, 0),    // rouge
        QColor(0, 255, 0),    // vert
        QColor(0, 0, 255),    // bleu
        QColor(255, 255, 0),  // jaune
        QColor(255, 0, 255),  // magenta
        QColor(0, 255, 255),  // cyan
        QColor(255, 128, 0),  // orange
    };
    QColor color = levelColors[depth % (sizeof(levelColors) / sizeof(QColor))];

    shader->bind();

    shader->setUniformValue("material.albedo", QVector3D(color.redF(), color.greenF(), color.blueF()));
    Render(node->bounds); // Render the bounding box
    shader->release();

    RenderBVH(shader, node->left.get(), depth + 1); // Render left child
    RenderBVH(shader, node->right.get(), depth + 1); // Render right child
}