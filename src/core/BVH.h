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
    if (objects.empty() || !objects[0]) {
        return AABB(); // Return an empty AABB if no objects are present
    }

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
            if (!a || !b) return false;
            return a->GetAABB().center[axis] < b->GetAABB().center[axis];
        });

        size_t length = objects.size();

        std::vector<AABB> leftBounds(length);
        std::vector<AABB> rightBounds(length);

        AABB left;
        bool leftValid = false;
        for (size_t i = 0; i < length; ++i) {
            if (!objects[i]) continue;
            if (!leftValid) {
                left = objects[i]->GetAABB();
                leftValid = true;
            } else {
                left = MergeAABB(left, objects[i]->GetAABB());
            }
            leftBounds[i] = left;
        }
        
        AABB right;
        bool rightValid = false;
        for (size_t i = length; i-- > 0;) {
            if (!objects[i]) continue;
            if (!rightValid) {
                right = objects[i]->GetAABB();
                rightValid = true;
            } else {
                right = MergeAABB(right, objects[i]->GetAABB());
            }
            rightBounds[i] = right;
        }

        for (size_t i = 1; i < length; ++i) {
            float leftArea = leftBounds[i - 1].SurfaceArea();
            float rightArea = rightBounds[i].SurfaceArea();

            float cost = leftArea * i + rightArea * (length - i);

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
    // Remove null objects
    objects.erase(std::remove_if(objects.begin(), objects.end(), [](const std::shared_ptr<T>& obj) {
        return !obj;
    }), objects.end());
    
    // If no objects are left, return nullptr
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
        if (!a || !b) return false;
        return a->GetAABB().center[axis] < b->GetAABB().center[axis];
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