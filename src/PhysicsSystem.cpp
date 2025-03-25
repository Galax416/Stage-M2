#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem()
{

}

void PhysicsSystem::Update(float deltaTime)
{
    // Apply forces on the object
    for (int i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->ApplyForces();
        bodies[i]->Update(deltaTime);
    }

    // Apply spring forces
    for (int i = 0, size = springs.size(); i < size; ++i) {
        springs[i].ApplyForce(deltaTime);
    }

    // Solve constraints
    for (int i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->SolveConstraints(constraints);
    }

}

void PhysicsSystem::Render(QOpenGLShaderProgram* shaderProgram)
{
    // Render rigidbodies
    for (int i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->Render(shaderProgram);
    }

    // Render springs
    for (int i = 0, size = springs.size(); i < size; ++i) {
        springs[i].Render(shaderProgram);
    }
    
}

void PhysicsSystem::ChangeGravityParticle(const QVector3D& g)
{
    for (int i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->SetGravity(g);
    }
        
    
}

void PhysicsSystem::ChangeFrictionParticle(float f)
{
    for (int i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->SetFriction(f);
    }
}
