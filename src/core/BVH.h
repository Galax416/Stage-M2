#pragma once

#include <memory>
#include <vector>
#include "Rigidbody.h"
#include "BoundingBox.h"

static inline AABB MergeAABB(const AABB& a, const AABB& b) 
{
    QVector3D minA = GetMin(a), maxA = GetMax(a);
    QVector3D minB = GetMin(b), maxB = GetMax(b);

    QVector3D newMin(qMin(minA.x(), minB.x()), qMin(minA.y(), minB.y()), qMin(minA.z(), minB.z()));
    QVector3D newMax(qMax(maxA.x(), maxB.x()), qMax(maxA.y(), maxB.y()), qMax(maxA.z(), maxB.z()));

    return FromMinMax(newMin, newMax);
}

template <typename T>
static inline AABB ComputeBVHBounds(const std::vector<std::shared_ptr<T>>& objects) 
{
    AABB bounds = objects[0]->GetAABB();
    for (size_t i = 1; i < objects.size(); ++i)
        bounds = MergeAABB(bounds, objects[i]->GetAABB());
    return bounds;
}

template <typename T>
struct BVHNode 
{
    AABB bounds;
    std::shared_ptr<T> object = nullptr; 

    std::unique_ptr<BVHNode<T>> left = nullptr;
    std::unique_ptr<BVHNode<T>> right = nullptr;

    bool isLeaf() const { return object != nullptr; }
};

struct BVHSplit 
{
    int axis;
    size_t index;
    float cost;
};

template <typename T>
inline BVHSplit FindBestSplit(std::vector<std::shared_ptr<T>>& objects)
{
    BVHSplit bestSplit = { 0, 0, std::numeric_limits<float>::infinity() };

    for (int axis = 0; axis < 3; ++axis) {
        std::sort(objects.begin(), objects.end(), [axis](const auto& a, const auto& b) {
            return a->GetAABB().position[axis] < b->GetAABB().position[axis];
        });

        std::vector<AABB> leftBounds(objects.size());
        std::vector<AABB> rightBounds(objects.size());

        AABB left = objects[0]->GetAABB();
        leftBounds[0] = left;

        for (size_t i = 1; i < objects.size(); ++i) {
            left = MergeAABB(left, objects[i]->GetAABB());
            leftBounds[i] = left;
        }

        AABB right = objects.back()->GetAABB();
        rightBounds[objects.size() - 1] = right;

        for (int i = static_cast<int>(objects.size()) - 2; i >= 0; --i) {
            right = MergeAABB(right, objects[i]->GetAABB());
            rightBounds[i] = right;
        }

        for (size_t i = 1; i < objects.size(); ++i) {
            float leftArea = leftBounds[i - 1].SurfaceArea();
            float rightArea = rightBounds[i].SurfaceArea();

            float cost = leftArea * i + rightArea * (objects.size() - i);

            if (cost < bestSplit.cost) {
                bestSplit = { axis, i, cost };
            }
        }
    }

    return bestSplit;
}

template <typename T>
inline std::unique_ptr<BVHNode<T>> BuildBVH(std::vector<std::shared_ptr<T>>& objects)
{
    if (objects.empty()) return nullptr;

    auto node = std::make_unique<BVHNode<T>>();
    if (objects.size() == 1) {
        node->object = objects[0];
        node->bounds = objects[0]->GetAABB();
        return node;
    }

    // Compute overall bounds
    node->bounds = ComputeBVHBounds(objects);

    // Find the best split
    BVHSplit best = FindBestSplit(objects);

    // Sort by center along axis
    std::sort(objects.begin(), objects.end(), [axis = best.axis](const auto& a, const auto& b) {
        return a->GetAABB().position[axis] < b->GetAABB().position[axis];
    });

    // Split objects
    auto midIter = objects.begin() + best.index;
    std::vector<std::shared_ptr<T>> left(objects.begin(), midIter);
    std::vector<std::shared_ptr<T>> right(midIter, objects.end());

    node->left = BuildBVH(left);
    node->right = BuildBVH(right);
    return node;
}

template <typename T>
void QueryBVH(const AABB& targetAABB, const BVHNode<T>* node, std::vector<std::shared_ptr<T>>& outObjects)
{
    if (!node) return;
    if (!node->bounds.Intersects(targetAABB)) return;

    if (node->isLeaf()) {
        outObjects.push_back(node->object);
        return;
    }

    QueryBVH(targetAABB, node->left.get(), outObjects);
    QueryBVH(targetAABB, node->right.get(), outObjects);
}