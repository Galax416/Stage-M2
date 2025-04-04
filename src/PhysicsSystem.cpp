#include "PhysicsSystem.h"

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

    // Solve constraints
    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->SolveConstraints(constraints);
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
    
}

void PhysicsSystem::ChangeGravity(const QVector3D& g)
{
    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->SetGravity(g);
    }
        
    
}

void PhysicsSystem::ChangeFriction(float f)
{
    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        bodies[i]->SetFriction(f);
    }
}

void PhysicsSystem::RotateRigidbodies(QVector3D rotation)
{
    if (rotation == QVector3D(0, 0, 0)) return; // No rotation

    QQuaternion qRotation = QQuaternion::fromEulerAngles(rotation);

    for (size_t i = 0, size = bodies.size(); i < size; ++i) {
        if (bodies[i]->type != RIGIDBODY_TYPE_PARTICLE) continue; // Only rotate Particles
        QVector3D initialPosition = rigidbodyPositions[i];

        initialPosition = qRotation * initialPosition;

        bodies[i]->SetPosition(initialPosition);

    }
    
}

void ConvertModelToParticleSprings(Model* model, std::vector<std::unique_ptr<Particle>> &particles, std::vector<std::unique_ptr<Spring>> &springs)
{
    // Convert the model into a set of particles and springs
    if (model == nullptr || model->mesh == nullptr) return;

    particles.clear();
    springs.clear();

    float mass = 10.0f; // Default mass for particles
    float radius = 1.0f; // Default radius for particles
    float stiffness = 10.0f; // Default stiffness for springs

    // Add default variables to the custom loader
    model->customOBJ->variables["default_mass"] = mass;
    model->customOBJ->variables["default_radius"] = radius;
    model->customOBJ->variables["default_stiffness"] = stiffness;

    // Each vertex in the model becomes a particle
    for (size_t i = 0, size = model->customOBJ->vertices.size(); i < size; ++i) {
        const QVector3D vertex = model->customOBJ->vertices[i];
        particles.push_back(std::make_unique<Particle>(vertex, radius, mass));
        model->customOBJ->nodes.push_back(Node{ static_cast<int>(i), radius, mass });
    }

    // Each edge in the model becomes a spring link
    for (size_t i = 0, size = model->customOBJ->faces.size(); i < size; ++i) {
        const Face& face = model->customOBJ->faces[i];
        for (size_t j = 0; j < face.vertexIndices.size(); ++j) {
            int vertexIndexA = face.vertexIndices[j];
            int vertexIndexB = face.vertexIndices[(j + 1) % face.vertexIndices.size()]; // Wrap around to the first vertex

            auto spring = std::make_unique<Spring>(stiffness, 1.0f, 0.0f);
            spring->SetParticles(particles[vertexIndexA].get(), particles[vertexIndexB].get());
            springs.push_back(std::move(spring));

            model->customOBJ->springLinks.push_back(SpringLink{ vertexIndexA, vertexIndexB, stiffness });
        }
    }
}

void ChargeModelParticleSprings(Model* model, std::vector<std::unique_ptr<Particle>> &particles, std::vector<std::unique_ptr<Spring>> &springs)
{
    // Charge the model into a set of particles and springs
    if (model == nullptr) return;

    particles.clear();
    springs.clear();

    // Charge particles
    for (const auto& node : model->customOBJ->nodes) {
        QVector3D position = model->customOBJ->vertices[node.vertexIndex];
        auto particle = std::make_unique<Particle>(position, node.radius, node.mass);
        particles.push_back(std::move(particle));
    }

    // Charge springs
    for (const auto& link : model->customOBJ->springLinks) {
        auto spring = std::make_unique<Spring>(link.stiffness, 1.0f, 0.0f);
        auto A = particles[link.nodeA].get();
        auto B = particles[link.nodeB].get();
        spring->SetParticles(A, B);
        springs.push_back(std::move(spring));
    }
}