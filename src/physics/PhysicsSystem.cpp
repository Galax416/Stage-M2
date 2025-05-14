#include "PhysicsSystem.h"
#include "CollisionSolver.h"
#include "Render.h"
#include "Model.h"

PhysicsSystem::PhysicsSystem()
{
    ClearAll();
}

/*
void PhysicsSystem::Update(float deltaTime)
{
    // Apply forces on the object
    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->ApplyForces();
        bodies[i]->Update(deltaTime);
    }

    // Apply spring forces
    for (size_t i = 0, size = springs.size(); i < size; ++i) {
        for (int step = 0, substeps = 5; step < substeps; ++step) {
            springs[i]->ApplyForce(deltaTime / substeps);
        }
        // springs[i]->ApplyForce(deltaTime);
    }

    // Update BVH for collision detection
    bvhRigidbodies = BuildBVH(constraints);
    bvhTriangleColliders = BuildBVH(triangleColliders);

    // Solve constraints
    const int constraintIterations = 4;
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
*/

void PhysicsSystem::Update(float deltaTime)
{
    // Thread-safe body/spring/constraint registration
    {
        QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
        for (auto& b : pendingBodies) bodies.push_back(b);
        pendingBodies.clear();

        for (auto& s : pendingSprings) springs.push_back(s);
        pendingSprings.clear();

        for (auto& c : pendingConstraints) constraints.push_back(c);
        pendingConstraints.clear();

        for (auto& t : pendingTriangleColliders) triangleColliders.push_back(t);
        pendingTriangleColliders.clear();
    }
    
    // Step 1: Predict positions
    for (auto& body : bodies) {
        if (body->IsStatic()) continue;

        QVector3D velocity = body->GetVelocity();
        velocity += body->GetGravity() * body->GetMass() * deltaTime;

        QVector3D tempPos = body->GetPosition();
        body->SetPosition(tempPos + (velocity * deltaTime));
        body->oldPosition = tempPos;

        body->SynsCollisionVolumes();

    }

    // Step 1.5: Reset lambdas
    for (auto& spring : springs)
        spring->ResetLambda();

    // Build BVH for collision detection
    SetUpBVH();

    // Step 2: Solve constraints
    const int constraintIterations = 20;
    for (int i = 0; i < constraintIterations; ++i) {
        for (auto& spring : springs) spring->SolveConstraints(deltaTime);

        for (auto& constaint : constraints) {
            std::vector<std::shared_ptr<Rigidbody>> nearbyRigdibodies;
            std::vector<std::shared_ptr<TriangleCollider>> nearbyTriangles;

            QueryBVH<Rigidbody>(constaint->GetAABB(), bvhRigidbodies.get(), nearbyRigdibodies);
            constaint->SolveConstraints(nearbyRigdibodies);

            QueryBVH<TriangleCollider>(constaint->GetAABB(), bvhTriangleColliders.get(), nearbyTriangles);
            constaint->SolveConstraints(nearbyTriangles);
        }
    }

    // Step 3: Update velocities (from position change)
    for (auto& body : bodies) {
        if (body->IsStatic()) continue;

        QVector3D velocity = (body->GetPosition() - body->oldPosition) / (deltaTime == 0.0f ? 1.0f : deltaTime);
        body->SetVelocity(velocity * body->GetFriction());
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
    if ( m_renderBVH && bvhRigidbodies ) RenderBVH(shaderProgram, bvhRigidbodies.get());
    // if ( m_renderBVH && bvhTriangleColliders ) RenderBVH(shaderProgram, bvhTriangleColliders.get());
    
}

void PhysicsSystem::ChangeGravity(const QVector3D& g)
{
    QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
    for (size_t i = 0, size = bodies.size(); i < size; ++i) bodies[i]->SetGravity(g);
}

void PhysicsSystem::ChangeFriction(float f)
{
    QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
    for (size_t i = 0, size = bodies.size(); i < size; ++i) bodies[i]->SetFriction(f);
}

void PhysicsSystem::RotateRigidbodies(QVector3D rotation)
{
    QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
    QQuaternion qRotation = QQuaternion::fromEulerAngles(rotation);

    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        QVector3D initialPosition = rigidbodyTransformations[i].position;
        if (bodies[i]->GetType() == RIGIDBODY_TYPE_PARTICLE) {
            initialPosition = qRotation * initialPosition;
            bodies[i]->SetPosition(initialPosition);
        } else if (bodies[i]->GetType() == RIGIDBODY_TYPE_BOX) {
            QVector3D newPoisition = qRotation * initialPosition;
            QQuaternion newRotation = qRotation * rigidbodyTransformations[i].rotation;

            bodies[i]->SetPosition(newPoisition);
            bodies[i]->SetRotation(newRotation);

            auto model = static_cast<Model*>(bodies[i].get());
            if (model) model->SynsCollisionVolumes();
        }

    }
    
}