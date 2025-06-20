#include "PhysicsSystem.h"
#include "CollisionSolver.h"
#include "Render.h"
#include "Model.h"

PhysicsSystem::PhysicsSystem()
{
    ClearAll();
}

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

        QVector3D tempPos = body->GetPosition();
        QVector3D acceleration = body->GetGravity() * body->GetMass();
        QVector3D velocity = (tempPos - body->oldPosition) * body->friction;

        QVector3D predictedPos = tempPos + velocity + acceleration * deltaTime * deltaTime;
        body->SetPosition(predictedPos);
        body->oldPosition = tempPos;

        body->SynsCollisionVolumes();

    }

    // Step 1.5: Reset lambdas
    for (auto& spring : springs) spring->ResetLambda();

    // Build BVH for collision detection
    SetUpBVH();

    // Step 2: Solve constraints
    const int constraintIterations = 30;
    for (int i = 0; i < constraintIterations; ++i) {
        for (auto& spring : springs) spring->SolveConstraints(deltaTime);

        for (auto& constaint : constraints) {
            std::vector<std::shared_ptr<Rigidbody>> nearbyRigdibodies;
            std::vector<std::shared_ptr<TriangleCollider>> nearbyTriangles;

            QueryBVH<TriangleCollider>(constaint->GetAABB(), bvhTriangleColliders.get(), nearbyTriangles);
            constaint->SolveConstraints(nearbyTriangles);
            
            QueryBVH<Rigidbody>(constaint->GetAABB(), bvhRigidbodies.get(), nearbyRigdibodies);
            constaint->SolveConstraints(nearbyRigdibodies);
        }
    }

    // for (auto& body : bodies) body->currentPosition = body->GetPosition();

    // m_lastStepTime = m_physicsTimer.elapsed();
}

void PhysicsSystem::Render(QOpenGLShaderProgram* shaderProgram, float alpha)
{
    // Render rigidbodies
    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        // qDebug() << "Position of body" << i << ":" << bodies[i]->GetPosition() << "Old Position:" << bodies[i]->oldPosition;
        // bodies[i]->displayPosition = (1.0f - alpha) * bodies[i]->previousPosition  + alpha * bodies[i]->currentPosition;
        // qDebug() << "Display Position of body" << i << ":" << bodies[i]->displayPosition;
        bodies[i]->Render(shaderProgram);

        if (m_renderCollider) bodies[i]->Rigidbody::Render(shaderProgram); // Render the rigidbody collider
        
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

// void PhysicsSystem::RotateRigidbodies(QVector3D rotation, const QVector3D& pivot)
// {
//     QMutexLocker locker(&m_dataMutex); // Lock the mutex for thread safety
//     QQuaternion qRotation = QQuaternion::fromEulerAngles(rotation);

//     for (size_t i = 0, size = bodies.size(); i < size; ++i) {
//         QVector3D initialPosition = rigidbodyTransformations[i].position;
//         // QQuaternion initialRotation = rigidbodyTransformations[i].rotation;

//         QVector3D relativePos = initialPosition - pivot;
//         QVector3D rotatedPos = qRotation * relativePos + pivot;
        
//         bodies[i]->SetPosition(rotatedPos);
//         bodies[i]->oldPosition = rotatedPos;
        
//         QQuaternion newRotation = qRotation;
//         bodies[i]->SetRotation(newRotation);

//         bodies[i]->SynsCollisionVolumes();
//     }
    
// }