#pragma once 

#include <vector>
#include <memory>

#include "Model.h"
#include "Particle.h"
#include "Spring.h"

void ConvertModelToParticleSprings(Model* model, std::vector<std::shared_ptr<Particle>> &particles, std::vector<std::shared_ptr<Spring>> &springs);
void ConvertParticleSpringsToModel(Model* model, std::vector<std::shared_ptr<Particle>> &particles, std::vector<std::shared_ptr<Spring>> &springs);
void ChargeModelParticleSprings(Model* model, std::vector<std::shared_ptr<Particle>> &particles, std::vector<std::shared_ptr<Spring>> &springs);