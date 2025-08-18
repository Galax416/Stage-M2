#pragma once

#include "Particle.h"

/*
Bending class represents a bending constraint between two triangles formed by four particles.
It calculates the dihedral angle between the two triangles and applies a constraint to maintain that angle.
*/
class Bending
{
private:
    std::shared_ptr<Particle> p1;
    std::shared_ptr<Particle> p2;
    std::shared_ptr<Particle> p3;
    std::shared_ptr<Particle> p4;

    float restingAngle  { 0.0f };
    double compliance   { 1.0f };
    double lambda       { 0.0f };

public:
    Bending(std::shared_ptr<Particle> a, std::shared_ptr<Particle> b, 
            std::shared_ptr<Particle> c, std::shared_ptr<Particle> d, 
            float k = 1.0f);

    void SolveConstraints(float dt);

    void ResetLambda() { lambda = 0.0f; }

    std::shared_ptr<Particle> GetP1() { return p1; }
    std::shared_ptr<Particle> GetP2() { return p2; }
    std::shared_ptr<Particle> GetP3() { return p3; }
    std::shared_ptr<Particle> GetP4() { return p4; }

    float GetRestingAngle() const { return restingAngle; }
    float GetCompliance() { return compliance; }

    void SetRestingAngle(float angle) { restingAngle = angle; }
    void SetCompliance(float c) { compliance = c; }

    float ComputeDihedralAngle() const;
};