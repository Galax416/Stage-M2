#include "ModelPhysicsConverter.h"
#include "CustomOBJLoader.h"
#include "Mesh.h"
#include "TriangleCollider.h"
#include "Utils.h"
#include <set>

void ConvertModelToParticleSprings(Model* model,
    std::vector<std::shared_ptr<Particle>> &particles,
    std::vector<std::shared_ptr<Spring>> &springs,
    std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders,
    bool edgeOnly)
{
    // Convert the model into a set of particles and springs
    if (model == nullptr || model->customOBJ == nullptr) return;

    float vertexOffset = particles.size(); // Offset for vertex indices

    float mass = 10.0f; // Default mass for particles
    float radius = 1.0f; // Default radius for particles
    float stiffness = 1000.0f; // Default stiffness for springs

    // Add default variables to the custom loader
    model->customOBJ->variables["default_mass"] = mass;
    model->customOBJ->variables["default_radius"] = radius;
    model->customOBJ->variables["default_stiffness"] = stiffness;

    // Each vertex in the model becomes a particle
    for (size_t i = 0, size = model->customOBJ->vertices.size(); i < size; ++i) {
        const QVector3D vertex = model->customOBJ->vertices[i];
        particles.push_back(std::make_shared<Particle>(vertex, radius, mass, true));
        model->customOBJ->nodes.push_back(Node{ static_cast<int>(i), radius, mass, true });
    }

    std::set<std::pair<int, int>> springSet; // To avoid duplicate springs

    for (const Face& face : model->customOBJ->originalFaces) {
        const auto& indices = face.vertexIndices;
        const size_t count = indices.size();

        for (size_t i = 0; i < count; ++i) {
            for (size_t j = i + 1; j < count; ++j) {
                int a = vertexOffset + indices[i];
                int b = vertexOffset + indices[j];

                bool isEdge = (j == i + 1) || (i == 0 && j == count - 1); // Check if it's an edge in the face
                if (edgeOnly && !isEdge) continue; // Skip if not an edge and edgeOnly is true

                std::pair<int, int> key = std::make_pair(std::min(a, b), std::max(a, b));
                if (springSet.find(key) != springSet.end()) continue;

                springSet.insert(key);

                auto spring = std::make_shared<Spring>(stiffness, 1.0f, 0.0f);
                auto A = particles[a].get();
                auto B = particles[b].get();
                if (A->GetColor() == Qt::white) A->SetColor(floatToQColor(stiffness /* + A->GetMass() */));
                if (B->GetColor() == Qt::white) B->SetColor(floatToQColor(stiffness /* + B->GetMass() */));
                spring->SetParticles(A, B);
                springs.push_back(spring);

                model->customOBJ->springLinks.push_back(SpringLink{ a, b, stiffness });
            }
        }
    }


    // Add triangle colliders for each face in the model
    for (const Face& face : model->customOBJ->faces) {
        if (face.vertexIndices.size() != 3) continue;
    
        int i0 = face.vertexIndices[0];
        int i1 = face.vertexIndices[1];
        int i2 = face.vertexIndices[2];

        auto p1 = particles[i0].get();
        auto p2 = particles[i1].get();
        auto p3 = particles[i2].get();
    
        auto collider = std::make_shared<TriangleCollider>(p1, p2, p3);
    
        triangleColliders.push_back(collider);
    }

}

void ConvertParticleSpringsToModel(Model* model, 
    std::vector<std::shared_ptr<Particle>> &particles, 
    std::vector<std::shared_ptr<Spring>> &springs)
{
    // Convert the particles and springs back into a model
    if (model == nullptr || model->customOBJ == nullptr) return;
    
    // Clear the model's customOBJ data
    model->customOBJ->clear();

    // Resize the customOBJ vectors to match the number of particles
    model->customOBJ->vertices.resize(particles.size());
    model->customOBJ->nodes.resize(particles.size());
    model->customOBJ->springLinks.resize(springs.size());

    // Convert particles to vertices / nodes
    for (size_t i = 0, size = particles.size(); i < size; ++i) {
        auto& particle = particles[i];
        model->customOBJ->vertices[i] = particle->transform.position;
        model->customOBJ->nodes[i].vertexIndex = i;
        model->customOBJ->nodes[i].radius = particle->GetRadius();
        model->customOBJ->nodes[i].mass = particle->GetMass();
        model->customOBJ->nodes[i].movable = particle->IsMovable();
    }

    // Convert springs to face / spring links
    for (size_t i = 0, size = springs.size(); i < size; ++i) {
        auto& spring = springs[i];
        auto p1 = spring->GetP1();
        auto p2 = spring->GetP2();

        // Find the indices of the particles in the model
        int indexA = std::distance(particles.begin(), std::find_if(particles.begin(), particles.end(), [&](const std::shared_ptr<Particle>& p) { return p.get() == p1; }));
        int indexB = std::distance(particles.begin(), std::find_if(particles.begin(), particles.end(), [&](const std::shared_ptr<Particle>& p) { return p.get() == p2; }));

        // Create a face (edge) ???

        // Create a face for the spring link
        model->customOBJ->springLinks[i].nodeA = indexA;
        model->customOBJ->springLinks[i].nodeB = indexB;

        model->customOBJ->springLinks[i].stiffness = spring->GetK(); // Use the spring's stiffness

    }

}

void ChargeModelParticleSprings(Model* model, 
    std::vector<std::shared_ptr<Particle>> &particles, 
    std::vector<std::shared_ptr<Spring>> &springs,
    std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders)
{
    // Charge the model into a set of particles and springs
    if (model == nullptr || model->customOBJ == nullptr) return;

    if (model->mesh) delete model->mesh; // Clean up the mesh if it exists
    model->mesh = nullptr; // Set the mesh pointer to null

    float vertexOffset = particles.size(); // Offset for vertex indices

    // Charge particles
    for (const auto& node : model->customOBJ->nodes) {
        QVector3D position = model->customOBJ->vertices[node.vertexIndex];
        auto particle = std::make_shared<Particle>(position, node.radius, node.mass, node.movable);
        particles.push_back(particle);
    }

    // Charge springs
    for (const auto& link : model->customOBJ->springLinks) {
        auto spring = std::make_shared<Spring>(link.stiffness, 1.0f, 0.0f);
        float a = link.nodeA + vertexOffset;
        float b = link.nodeB + vertexOffset;
        auto A = particles[a].get();
        auto B = particles[b].get();
        if (A->GetColor() == Qt::white) A->SetColor(floatToQColor(link.stiffness /* + A->GetMass() */));
        if (B->GetColor() == Qt::white) B->SetColor(floatToQColor(link.stiffness /* + B->GetMass() */));
        spring->SetParticles(A, B);
        springs.push_back(spring);
    }

    // Add triangle colliders for each face in the model
    for (const Face& face : model->customOBJ->faces) {
        if (face.vertexIndices.size() != 3) continue;
    
        int i0 = face.vertexIndices[0];
        int i1 = face.vertexIndices[1];
        int i2 = face.vertexIndices[2];

        auto p1 = particles[i0].get();
        auto p2 = particles[i1].get();
        auto p3 = particles[i2].get();
    
        auto collider = std::make_shared<TriangleCollider>(p1, p2, p3);
    
        triangleColliders.push_back(collider);
    }
}