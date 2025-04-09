#include "BVH.h"
#include "BoundingBox.h"


static AABB MergeAABB(const AABB& a, const AABB& b) {
    QVector3D minA = GetMin(a), maxA = GetMax(a);
    QVector3D minB = GetMin(b), maxB = GetMax(b);

    QVector3D newMin(qMin(minA.x(), minB.x()), qMin(minA.y(), minB.y()), qMin(minA.z(), minB.z()));
    QVector3D newMax(qMax(maxA.x(), maxB.x()), qMax(maxA.y(), maxB.y()), qMax(maxA.z(), maxB.z()));

    return FromMinMax(newMin, newMax);
}

static AABB ComputeBVHBounds(const std::vector<std::shared_ptr<Rigidbody>>& objects) {
    AABB bounds = objects[0]->GetAABB();
    for (size_t i = 1; i < objects.size(); ++i)
        bounds = MergeAABB(bounds, objects[i]->GetAABB());
    return bounds;
}

std::unique_ptr<BVHNode> BuildBVH(std::vector<std::shared_ptr<Rigidbody>>& bodies) {
    if (bodies.empty()) return nullptr;

    auto node = std::make_unique<BVHNode>();
    if (bodies.size() == 1) {
        node->body = bodies[0];
        node->bounds = bodies[0]->GetAABB();
        return node;
    }

    // Compute overall bounds
    node->bounds = ComputeBVHBounds(bodies);

    // Split axis: longest axis
    QVector3D size = node->bounds.size;
    int axis = 0;
    if (size.y() > size.x()) axis = 1;
    if (size.z() > size[axis]) axis = 2;

    // Sort by center along axis
    std::sort(bodies.begin(), bodies.end(), [axis](const auto& a, const auto& b) {
        return a->transform.position[axis] < b->transform.position[axis];
    });

    // Split bodies
    size_t mid = bodies.size() / 2;
    std::vector<std::shared_ptr<Rigidbody>> leftObjs(bodies.begin(), bodies.begin() + mid);
    std::vector<std::shared_ptr<Rigidbody>> rightObjs(bodies.begin() + mid, bodies.end());

    node->left = BuildBVH(leftObjs);
    node->right = BuildBVH(rightObjs);
    return node;
}

void QueryBVH(const std::shared_ptr<Rigidbody>& target, BVHNode* node, std::vector<std::shared_ptr<Rigidbody>>& outCandidates) {
    if (!node) return;

    AABB targetAABB = target->GetAABB();
    if (!node->bounds.Intersects(targetAABB)) return;

    if (node->isLeaf()) {
        if (node->body != target) {
            outCandidates.push_back(node->body);
        }
        return;
    }

    QueryBVH(target, node->left.get(), outCandidates);
    QueryBVH(target, node->right.get(), outCandidates);
}
