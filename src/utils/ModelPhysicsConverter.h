#pragma once 

#include <vector>
#include <memory>

#include "Model.h"
#include "Particle.h"
#include "Spring.h"

void RemoveDuplicateVerticesAndFaces(CustomOBJLoader* obj);

void ConvertModelToParticleSprings(std::shared_ptr<Model> &model,
    std::vector<std::shared_ptr<Particle>> &particles,
    std::vector<std::shared_ptr<Spring>> &springs,
    std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders,
    bool edgeOnly = false
);

void ChargeModelParticleSprings(std::shared_ptr<Model> &model, 
    std::vector<std::shared_ptr<Particle>> &particles, 
    std::vector<std::shared_ptr<Spring>> &springs,
    std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders,
    bool edgeOnly = false
);

void ConvertParticleSpringsToModel(std::shared_ptr<Model> &model, 
    const std::vector<std::shared_ptr<Particle>> &particles, 
    const std::vector<std::shared_ptr<Spring>> &springs,
    const std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders
);

void UpdateModelFromParticles(std::shared_ptr<Model> &model);