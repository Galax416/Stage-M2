#include "PhysicsSystem.h"
#include "Render.h"

PhysicsSystem::PhysicsSystem()
{
    ClearAll();
}

void PhysicsSystem::Update(float deltaTime)
{
    // Apply forces on the object
    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->ApplyForces();
        bodies[i]->Update(deltaTime);
    }

    // Apply spring forces
    for (size_t i = 0, size = springs.size(); i < size; ++i) {
        springs[i].ApplyForce(deltaTime);
    }

    // Update BVH for collision detection
    bvhRoot = BuildBVH(constraints);

    // Solve constraints
    for (auto& body : constraints) {
        std::vector<std::shared_ptr<Rigidbody>> nearby;
        QueryBVH(body, bvhRoot.get(), nearby);
        body->SolveConstraints(nearby);
    }

}

void PhysicsSystem::Render(QOpenGLShaderProgram* shaderProgram)
{
    // Render rigidbodies
    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->Render(shaderProgram);
    }

    // Render springs
    for (size_t i = 0, size = springs.size(); i < size; ++i) {
        springs[i].Render(shaderProgram);
    }

    // Render BVH (debug)
    // RenderBVH(shaderProgram, bvhRoot.get());
    
}

void PhysicsSystem::ChangeGravity(const QVector3D& g)
{
    for (size_t i = 0, size = bodies.size(); i < size; ++i) bodies[i]->SetGravity(g);
}

void PhysicsSystem::ChangeFriction(float f)
{
    for (size_t i = 0, size = bodies.size(); i < size; ++i) bodies[i]->SetFriction(f);
}

void PhysicsSystem::RotateRigidbodies(QVector3D rotation)
{
    QQuaternion qRotation = QQuaternion::fromEulerAngles(rotation);

    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        if (bodies[i]->type != RIGIDBODY_TYPE_PARTICLE) continue; // Only rotate Particles
        QVector3D initialPosition = rigidbodyPositions[i];

        initialPosition = qRotation * initialPosition;

        bodies[i]->SetPosition(initialPosition);

    }
    
}