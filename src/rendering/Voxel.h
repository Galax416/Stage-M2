#pragma once

#include <QVector3D>
#include <QDebug>
#include <vector>
#include <array>
#include <memory>
#include <set>

#include "MaterialPropierties.h"


// Foward declarations
class Particle;
class Spring;

struct VoxelSpring
{
    MaterialType materialType;

    std::array<std::shared_ptr<Particle>, 8> corners;
    std::vector<std::shared_ptr<Spring>> springs;
    QVector3D index3D;

};

class VoxelGrid
{
public:
    std::vector<std::shared_ptr<Particle>> particles;
    std::vector<VoxelSpring> voxelSprings;

    QVector3D origin;
    QVector3D size;
    float spacing; 

    std::vector<MaterialType> materialTypes; // List of material types
    unsigned int nbTypes = 0; // Number of types of materials

    VoxelGrid() = default;
    VoxelGrid(const QVector3D& _origin, const QVector3D& _size, float _spacing)
        : origin(_origin), size(_size), spacing(_spacing) {}

    void Generate();

    std::vector<std::shared_ptr<Particle>> GetParticles() const { return particles; }
    std::vector<std::shared_ptr<Spring>> GetSprings() const;

    void SetCross(bool isCross) { m_isCross = isCross; }

private:
    bool m_isCross { false };

    int ParticleIndex(int x, int y, int z) const
    {
        return x + (size.x() + 1) * (y + (size.y() + 1) * z);
    }

    void UpdateNumbersOfTypes()
    {
        std::set<MaterialType> uniqueTypes;
        for (const auto& voxel : voxelSprings) {
            uniqueTypes.insert(voxel.materialType);
        }
        
        nbTypes = uniqueTypes.size();
        for (const auto& type : uniqueTypes) materialTypes.push_back(type);
    }

    std::vector<std::vector<int>> getSprings(bool isCross) {
        using Edge = std::vector<int>;

        static const std::vector<Edge> basicSprings {
            {0,1},{1,5},{5,4},{4,0}, // bottom face
            {2,3},{3,7},{7,6},{6,2}, // top face
            {0,2},{1,3},{4,6},{5,7}  // vertical edges
        };

        static const std::vector<Edge> crossSprings {
            {0,1},{1,5},{5,4},{4,0}, // bottom face
            {2,3},{3,7},{7,6},{6,2}, // top face
            {0,2},{1,3},{4,6},{5,7}, // vertical edges
            {0,3},{1,2},{4,7},{5,6}, // diagonal vertical edges
            {0,5},{1,4},{2,7},{3,6}, // diagonal horizontal edges
            {0,6},{1,7},{2,4},{3,5}, // diagonal front/back edges
            {0,7},{1,6},{2,5},{3,4}  // diagonal internal
        };

        return isCross ? crossSprings : basicSprings;
    }
    
};

// k = (E * A) / L with E = Young's modulus, A = cross-sectional area, L = length of the spring
inline float ComputeSpringStiffness(float E, float L, float A) { return (E * A) / L; }

// Exmple
inline bool IsCross(int x, int y, int z, int centerX, int centerY, int centerZ, int thickness = 1) {
    return (
        (abs(x - centerX) <= thickness && y == centerY && z == centerZ) ||  // axe X
        (x == centerX && abs(y - centerY) <= thickness && z == centerZ) ||  // axe Y
        (x == centerX && y == centerY && abs(z - centerZ) <= thickness)     // axe Z
    );
}


