#pragma once

#include <memory>
#include <vector>
#include "Rigidbody.h"

struct BVHNode {
    AABB bounds;
    std::shared_ptr<Rigidbody> body = nullptr; 

    std::unique_ptr<BVHNode> left = nullptr;
    std::unique_ptr<BVHNode> right = nullptr;

    bool isLeaf() const { return body != nullptr; }
};

std::unique_ptr<BVHNode> BuildBVH(std::vector<std::shared_ptr<Rigidbody>>& bodies);

void QueryBVH(const std::shared_ptr<Rigidbody>& target, BVHNode* node, std::vector<std::shared_ptr<Rigidbody>>& outCandidates);
