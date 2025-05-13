#include "Voxel.h"
#include "Particle.h"
#include "Spring.h"

#include <map>

std::vector<std::shared_ptr<Spring>> VoxelGrid::GetSprings() const
{ 
    using ParticlePair = std::pair<std::shared_ptr<Particle>, std::shared_ptr<Particle>>;
    std::map<ParticlePair, std::shared_ptr<Spring>> uniqueSpringsMap;

    for (const auto& voxel : voxelSprings) {
        for (const auto& spring : voxel.springs) {
            auto p1 = spring->GetP1();
            auto p2 = spring->GetP2();

            if (!p1 || !p2) continue;

            auto key = std::minmax(p1, p2); // Create a unique key for the pair of particles

            auto it = uniqueSpringsMap.find(key);
            if (it == uniqueSpringsMap.end() || spring->GetStiffness() > it->second->GetStiffness()) {
                uniqueSpringsMap[key] = spring; // Store the spring with the highest stiffness
            }
        }
    }

    std::vector<std::shared_ptr<Spring>> result;
    result.reserve(uniqueSpringsMap.size());
    for (const auto& pair : uniqueSpringsMap) result.push_back(pair.second);
    return result;
}

void VoxelGrid::Generate()
{
    float sizeX = size.x();
    float sizeY = size.y();
    float sizeZ = size.z();

    if (sizeX <= 0 || sizeY <= 0 || sizeZ <= 0 || spacing <= 0) return;
    
    particles.clear();
    voxelSprings.clear();

    float radius = 3.0f;

    QVector3D origin = QVector3D(-sizeX * spacing / 2.0f, -sizeY * spacing / 2.0f, -sizeZ * spacing / 2.0f);

    // Creat grid of particles
    for (int z = 0; z <= sizeZ; ++z) {
        for (int y = 0; y <= sizeY; ++y) {
            for (int x = 0; x <= sizeX; ++x) {
                QVector3D pos = origin + QVector3D(x * spacing, y * spacing, z * spacing);
                // auto p = std::make_shared<Particle>(pos, radius, 0.0f);
                particles.push_back(std::make_shared<Particle>(pos, radius, 0.0f));
            }
        }
    }

    int centerX = sizeX / 2;
    int centerY = sizeY / 2;
    int centerZ = sizeZ / 2;

    // Build voxel springs
    for (int z = 0; z < sizeZ; ++z) {
        for (int y = 0; y <sizeY; ++y) {
            for (int x = 0; x < sizeX; ++x) {

                VoxelSpring vs;
                vs.index3D = QVector3D(x, y, z);
                

                vs.materialType = IsCross(x, y, z, centerX, centerY, centerZ, 1) ? MaterialType_Metal : MaterialType_Rock;
                
                MaterialProperties materialProps = GetMaterialProperties(vs.materialType);
                float volume = 0.01f; // spacing * spacing * spacing; // Volume of the voxel
                float voxelMass = materialProps.density * volume; // Mass of the voxel
                
                // Corner indices
                int idx[8] = {
                    ParticleIndex(x, y, z),     ParticleIndex(x, y, z+1),
                    ParticleIndex(x, y+1, z),   ParticleIndex(x, y+1, z+1),
                    ParticleIndex(x+1, y, z),   ParticleIndex(x+1, y, z+1),
                    ParticleIndex(x+1, y+1, z), ParticleIndex(x+1, y+1, z+1)
                };

                for (int i = 0; i < 8; ++i) {
                    auto& p = particles[idx[i]];
                    float m = p->GetMass();
                    p->SetMass(m + voxelMass / 8.0f); // Distribute mass to corners
                    // qDebug() << "Particle mass: " << p->GetMass();
                    vs.corners[i] = p; // Store the corner particle
                }

                // std::vector<std::vector<int>> springs = getSprings(m_isCross);

                // const int springs[28][2] = {
                //     {0,1},{1,5},{5,4},{4,0}, // bottom face
                //     {2,3},{3,7},{7,6},{6,2}, // top face
                //     {0,2},{1,3},{4,6},{5,7}, // vertical edges
                //     {0,3},{1,2},{4,7},{5,6}, // diagonal vertical edges
                //     {0,5},{1,4},{2,7},{3,6}, // diagonal horizontal edges
                //     {0,6},{1,7},{2,4},{3,5}, // diagonal front/back edges
                //     {0,7},{1,6},{2,5},{3,4}  // diagonal intern
                // };

                // float E = materialProps.E; // Young's modulus of the voxel
                // float A = spacing * spacing / 28.0f; // Effective cross-sectional area per spring

                // for (const auto& spring : springs) {
                    // auto p1 = vs.corners[spring[0]];
                    // auto p2 = vs.corners[spring[1]];

                    // float L = (p2->GetPosition() - p1->GetPosition()).length();

                    // float K = ComputeSpringStiffness(E, L, A); 
                    // float K = vs.materialType == MaterialType_Bone ? 200.0f : 90.0f; // Example stiffness values

                    // auto s = std::make_shared<Spring>(K);
                    // s->SetParticles(p1, p2);
                    // s->SetColor(floatToQColor(vs.materialType));
                    // vs.springs.push_back(s);
                // }

                voxelSprings.push_back(vs);
            }
        }
    }

    UpdateNumbersOfTypes(); // Update the number of types of materials
}