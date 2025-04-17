#include "PhysicsSystem.h"
#include "CollisionSolver.h"
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
        for (int step = 0, substeps = 4; step < substeps; ++step) {
            springs[i]->ApplyForce(deltaTime / substeps);
        }
        // springs[i]->ApplyForce(deltaTime);
    }

    // Update BVH for collision detection
    bvhRigidbodies = BuildBVH(constraints);
    bvhTriangleColliders = BuildBVH(triangleColliders);

    // Solve constraints
    const int constraintIterations = 5;
    for (int iter = 0; iter < constraintIterations; ++iter) {
        for (auto& constraint : constraints) {
            std::vector<std::shared_ptr<Rigidbody>> nearbyRigdibodies;
            std::vector<std::shared_ptr<TriangleCollider>> nearbyTriangles;

            QueryBVH<Rigidbody>(constraint->GetAABB(), bvhRigidbodies.get(), nearbyRigdibodies);
            constraint->SolveConstraints(nearbyRigdibodies);

            QueryBVH<TriangleCollider>(constraint->GetAABB(), bvhTriangleColliders.get(), nearbyTriangles);
            constraint->SolveConstraints(nearbyTriangles);
        }
        
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
        springs[i]->Render(shaderProgram);
    }

    // Render BVH (debug)
    if ( m_renderBVH ) RenderBVH(shaderProgram, bvhRigidbodies.get());
    
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