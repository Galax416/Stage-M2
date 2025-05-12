#pragma once 

#include <vector>
#include <memory>

#include "Model.h"
#include "Particle.h"
#include "Spring.h"

void RemoveDuplicateVerticesAndFaces(CustomOBJLoader* obj);

void ConvertModelToParticleSprings(Model* model,
    std::vector<std::shared_ptr<Particle>> &particles,
    std::vector<std::shared_ptr<Spring>> &springs,
    std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders,
    bool edgeOnly = false
);

/*void ConvertParticleSpringsToModel(Model* model, 
    std::vector<std::shared_ptr<Particle>> &particles, 
    std::vector<std::shared_ptr<Spring>> &springs
);*/

void ChargeModelParticleSprings(Model* model, 
    std::vector<std::shared_ptr<Particle>> &particles, 
    std::vector<std::shared_ptr<Spring>> &springs,
    std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders,
    bool edgeOnly = false
);