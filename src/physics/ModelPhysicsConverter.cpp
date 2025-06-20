#include "ModelPhysicsConverter.h"
#include "CustomOBJLoader.h"
#include "Mesh.h"
#include "TriangleCollider.h"
#include "Utils.h"
#include <set>

void RemoveDuplicateVerticesAndFace(CustomOBJLoader* obj)
{
    // Remove duplicate vertices and faces from the obj
    if (!obj) return;

    // Remove duplicate vertices
    std::vector<QVector3D>& vertices = obj->vertices;
    std::unordered_map<QVector3D, int> vertexMap;
    std::vector<QVector3D> uniqueVertices;
    std::vector<int> vertexIndices(vertices.size(), -1);

    for (size_t i = 0; i < vertices.size(); ++i) {
        const QVector3D& vertex = vertices[i];
        if (vertexMap.find(vertex) == vertexMap.end()) {
            vertexMap[vertex] = uniqueVertices.size();
            uniqueVertices.push_back(vertex);
        }
        vertexIndices[i] = vertexMap[vertex];
    }
    vertices = uniqueVertices;

    // Update face vertex indices
    for (Face& face : obj->faces) {
        for (int& index : face.vertexIndices) {
            index = vertexIndices[index];
        }
    }
    for (Face& face : obj->originalFaces) {
        for (int& index : face.vertexIndices) {
            index = vertexIndices[index];
        }
    }

    // Remove duplicate faces
    std::set<std::vector<int>> faceSet;
    std::vector<Face> uniqueFaces;

    for (const Face& face : obj->faces) {
        std::vector<int> sortedIndices = face.vertexIndices;
        std::sort(sortedIndices.begin(), sortedIndices.end());
        if (faceSet.find(sortedIndices) == faceSet.end()) {
            faceSet.insert(sortedIndices);
            uniqueFaces.push_back(face);
        }
    }
    obj->faces = uniqueFaces;

}

void ConvertModelToParticleSprings(std::shared_ptr<Model> &model,
    std::vector<std::shared_ptr<Particle>> &particles,
    std::vector<std::shared_ptr<Spring>> &springs,
    std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders,
    bool edgeOnly)
{
    // Convert the model into a set of particles and springs
    if (model == nullptr || model->customOBJ == nullptr) return;

    // Remove duplicate vertices and faces
    RemoveDuplicateVerticesAndFace(model->customOBJ.get());

    // Clear vectors
    particles.clear();
    springs.clear();
    triangleColliders.clear();

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
                int a = indices[i];
                int b = indices[j];

                bool isEdge = (j == i + 1) || (i == 0 && j == count - 1); // Check if it's an edge in the face
                if (edgeOnly && !isEdge) continue; // Skip if not an edge and edgeOnly is true

                std::pair<int, int> key = std::make_pair(std::min(a, b), std::max(a, b));
                if (springSet.find(key) != springSet.end()) continue;

                springSet.insert(key);

                auto spring = std::make_shared<Spring>(particles[a], particles[b], stiffness);
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

        auto p1 = particles[i0];
        p1->AddFlag(ParticleFlags::PARTICLE_NO_COLLISION_WITH_US);
        p1->AddFlag(ParticleFlags::PARTICLE_ATTACHED_TO_TRIANGLE);
        auto p2 = particles[i1];
        p2->AddFlag(ParticleFlags::PARTICLE_NO_COLLISION_WITH_US);
        p2->AddFlag(ParticleFlags::PARTICLE_ATTACHED_TO_TRIANGLE);
        auto p3 = particles[i2];
        p3->AddFlag(ParticleFlags::PARTICLE_NO_COLLISION_WITH_US);
        p3->AddFlag(ParticleFlags::PARTICLE_ATTACHED_TO_TRIANGLE);
    
        auto collider = std::make_shared<TriangleCollider>(p1, p2, p3);
    
        triangleColliders.push_back(collider);
    }

}

/*void ConvertParticleSpringsToModel(Model* model, 
    std::vector<std::shared_ptr<Particle>> &particles, 
    std::vector<std::shared_ptr<Spring>> &springs)
{
    // Convert the particles and springs back into a model
    if (model == nullptr || model->customOBJ == nullptr) return;

    if (model->mesh) delete model->mesh; // Clean up the mesh if it exists
    model->mesh = nullptr; // Set the mesh pointer to null
    
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

}*/

void ChargeModelParticleSprings(std::shared_ptr<Model> &model, 
    std::vector<std::shared_ptr<Particle>> &particles, 
    std::vector<std::shared_ptr<Spring>> &springs,
    std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders,
    bool edgeOnly)
{
    // Charge the model into a set of particles and springs
    if (model == nullptr || model->customOBJ == nullptr) return;

    // if (model->mesh) delete model->mesh; // Clean up the mesh if it exists
    // model->mesh = nullptr; // Set the mesh pointer to null

    // Clear vectors
    particles.clear();
    springs.clear();
    triangleColliders.clear();

    // Charge particles
    for (const auto& node : model->customOBJ->nodes) {
        QVector3D position = model->customOBJ->vertices[node.vertexIndex];
        auto particle = std::make_shared<Particle>(position, node.radius, node.mass, node.movable);
        particles.push_back(particle);
    }

    // Verify if the spring is on the edge of the face
    std::set<std::pair<int, int>> validEdges;
    if (edgeOnly) {
        for (const auto& face : model->customOBJ->originalFaces) {
            const auto& indices = face.vertexIndices;
            size_t count = indices.size();
            for (size_t i = 0; i < count; ++i) {
                int a = indices[i];
                int b = indices[(i + 1) % count];
                validEdges.insert({ std::min(a, b), std::max(a, b) });
            }
        }
    }

    // Charge springs
    for (const auto& link : model->customOBJ->springLinks) {
        int a = link.nodeA;
        int b = link.nodeB;

        if (edgeOnly) {
            std::pair<int, int> key = { std::min(a, b), std::max(a, b) };
            if (validEdges.find(key) == validEdges.end()) {
                continue; // Skip si ce n'est pas une arête réelle
            }
        }

        int indexA = a;
        int indexB = b;
        auto spring = std::make_shared<Spring>(particles[indexA], particles[indexB], link.stiffness);
        springs.push_back(spring);
    }

    // Add triangle colliders for each face in the model
    for (const Face& face : model->customOBJ->faces) {
        if (face.vertexIndices.size() != 3) continue;
    
        int i0 = face.vertexIndices[0];
        int i1 = face.vertexIndices[1];
        int i2 = face.vertexIndices[2];

        auto p1 = particles[i0];
        p1->AddFlag(ParticleFlags::PARTICLE_NO_COLLISION_WITH_US);
        p1->AddFlag(ParticleFlags::PARTICLE_ATTACHED_TO_TRIANGLE);
        auto p2 = particles[i1];
        p2->AddFlag(ParticleFlags::PARTICLE_NO_COLLISION_WITH_US);
        p2->AddFlag(ParticleFlags::PARTICLE_ATTACHED_TO_TRIANGLE);
        auto p3 = particles[i2];
        p3->AddFlag(ParticleFlags::PARTICLE_NO_COLLISION_WITH_US);
        p3->AddFlag(ParticleFlags::PARTICLE_ATTACHED_TO_TRIANGLE);
    
        auto collider = std::make_shared<TriangleCollider>(p1, p2, p3);
    
        triangleColliders.push_back(collider);
    }
}

void ConvertParticleSpringsToModel(std::shared_ptr<Model> &model, 
    std::vector<std::shared_ptr<Particle>> &particles, 
    std::vector<std::shared_ptr<Spring>> &springs,
    std::vector<std::shared_ptr<TriangleCollider>> &triangleColliders)
{
    // Convert the particles and springs back into a model
    if (!model) return;

    model->customOBJ->Clear(); // Clear the customOBJ data

    // Resize the customOBJ vectors to match the number of particles
    model->customOBJ->vertices.resize(particles.size());
    model->customOBJ->normals.resize(particles.size());
    model->customOBJ->nodes.resize(particles.size());
    model->customOBJ->springLinks.resize(springs.size());

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material; // Default material for the mesh

    // Create a mapping from particles to their indices
    std::unordered_map<std::shared_ptr<Particle>, int> particleIndexMap;
    for (size_t i = 0, size = particles.size(); i < size; ++i) {
        particleIndexMap[particles[i]] = static_cast<int>(i);
    }

    // Convert particles to vertices / nodes
    for (size_t i = 0, size = particles.size(); i < size; ++i) {
        auto& particle = particles[i];
        model->customOBJ->vertices[i] = particle->transform.position;
        model->customOBJ->nodes[i].vertexIndex = i;
        model->customOBJ->nodes[i].radius = particle->GetRadius() * 100.0f;
        model->customOBJ->nodes[i].mass = particle->GetMass();
        model->customOBJ->nodes[i].movable = particle->IsDynamic();

        // Create a vertex for the mesh
        Vertex vertex;
        vertex.position = particle->transform.position;
        vertex.normal = QVector3D(0.0f, 0.0f, 0.0f); // Default normal
        vertex.texCoords = QVector2D(0.0f, 0.0f); // Default texture coordinates
        vertices.push_back(vertex);
    }

    // Convert springs spring links
    for (size_t i = 0, size = springs.size(); i < size; ++i) {
        auto& spring = springs[i];
        auto p1 = spring->GetP1();
        auto p2 = spring->GetP2();

        auto it1 = particleIndexMap.find(p1);
        auto it2 = particleIndexMap.find(p2);

        if (it1 == particleIndexMap.end() || it2 == particleIndexMap.end()) {
            qWarning("Spring refers to unknown particles, skipping.");
            continue;
        }

        size_t index1 = it1->second;
        size_t index2 = it2->second;

        model->customOBJ->springLinks[i].nodeA = index1;
        model->customOBJ->springLinks[i].nodeB = index2;
        model->customOBJ->springLinks[i].stiffness = spring->GetStiffness();
    }

    // Convert triangle colliders to mesh indices
    for (const auto& collider : triangleColliders) {
        if (collider->p0 == nullptr || collider->p1 == nullptr || collider->p2 == nullptr) {
            qWarning("TriangleCollider has null particles, skipping.");
            continue;
        }

        int index1 = particleIndexMap[collider->p0];
        int index2 = particleIndexMap[collider->p1];
        int index3 = particleIndexMap[collider->p2];

        // Add indices for the triangle
        indices.push_back(index1);
        indices.push_back(index2);
        indices.push_back(index3);

        // Creat face
        Face face;
        face.vertexIndices = { index1, index2, index3 };
        model->customOBJ->faces.push_back(face);
    }

    model->mesh = std::make_shared<Mesh>(vertices, indices, material);
    for (size_t i = 0, size = model->mesh->vertices.size(); i < size; ++i) {
        model->customOBJ->normals[i] = model->mesh->vertices[i].normal;
    }

    model->triangleColliders = triangleColliders;
    
}