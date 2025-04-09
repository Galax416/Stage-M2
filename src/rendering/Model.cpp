#include "Model.h"
#include "Mesh.h"
#include "CustomOBJLoader.h"

Model::Model()
{
    Init();
}

Model::Model(const QString& path)
{
    Init();
    LoadModel(path);
}

Model::~Model()
{
    ClearModel();
}

void Model::ClearModel()
{
    if (mesh) delete mesh;
    mesh = nullptr;
    if (customOBJ) delete customOBJ;
    customOBJ = nullptr;
}

void Model::Init()
{
    initializeOpenGLFunctions();
    type = RIGIDBODY_TYPE_BOX;
    bounds = AABB();
    oldPosition = transform.position;
    customOBJ = new CustomOBJLoader();
}

void Model::SynsCollisionVolumes()
{
    if (type == RIGIDBODY_TYPE_SPHERE) {
        sphereCollider.position = transform.position;
        sphereCollider.radius = transform.scale.x();
    }
    else if (type == RIGIDBODY_TYPE_BOX) {
        boxCollider.position = bounds.position + transform.position;
        boxCollider.size = bounds.size * transform.scale;
        boxCollider.orientation = transform.GetRotationMatrix();
    }
}

void Model::SetUpColliders()
{
    if (mesh == nullptr) return;
    BuildAABB();
    SynsCollisionVolumes();
}

void Model::LoadModel(const QString& path)
{

    ClearModel();

    customOBJ = new CustomOBJLoader();

    bool ok = customOBJ->LoadOBJ(path.toStdString().c_str());
    if (!ok) {
        std::cerr << "Failed to load OBJ file: " << path.toStdString() << std::endl;
        return;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    // Process vertices
    for (size_t i = 0, size = customOBJ->vertices.size(); i < size; ++i) {
        Vertex vertex;
        vertex.position = customOBJ->vertices[i];
        vertices.push_back(vertex);
    }

    // Process normals
    for (size_t i = 0, size = customOBJ->normals.size(); i < size; ++i) {
        if (i < vertices.size()) {
            vertices[i].normal = customOBJ->normals[i];
        }
    }

    // Process texture coordinates
    for (size_t i = 0, size = customOBJ->texCoords.size(); i < size; ++i) {
        if (i < vertices.size()) {
            vertices[i].texCoords = customOBJ->texCoords[i];
        }
    }

    // Process faces
    for (const auto& face : customOBJ->faces) {
        for (size_t i = 0, size = face.vertexIndices.size(); i < size; ++i) {
            indices.push_back(face.vertexIndices[i]);
        }
    }

    mesh = new Mesh(vertices, indices, material);
    
    SetUpColliders();
}

void Model::Update(float deltaTime)
{
    if (!isMovable) return;

    QVector3D velocity = (transform.position - oldPosition);
    oldPosition = transform.position;
    float deltaSquare = deltaTime * deltaTime;
    
    if (fabsf(velocity.x()) < 0.001f) {
        velocity.setX(0.0f);
    }
    if (fabsf(velocity.y()) < 0.001f) {
        velocity.setY(0.0f);
    }
    if (fabsf(velocity.z()) < 0.001f) {
        velocity.setZ(0.0f);
    }

    // if (type == RIGIDBODY_TYPE_BOX) {
    //     QVector3D angAccel = torques * InvTensor();
    //     angVel += angAccel * deltaTime;
    //     angVel *= friction; // à voir !

    //     if (fabsf(angVel.x()) < 0.001f) {
    //         angVel.setX(0.0f);
    //     }
    //     if (fabsf(angVel.y()) < 0.001f) {
    //         angVel.setY(0.0f);
    //     }
    //     if (fabsf(angVel.z()) < 0.001f) {
    //         angVel.setZ(0.0f);
    //     }
    // }

    transform.position += (velocity * friction + forces * deltaSquare);

    // if (type == RIGIDBODY_TYPE_BOX) {
    //     orientation += angVel * deltaTime;
    //     transform.SetRotationEuler(orientation); // à voir !
    //     boxCollider.orientation = transform.GetRotationMatrix();
    // }

    SynsCollisionVolumes();

}

void Model::Render(QOpenGLShaderProgram* shaderProgram)
{
    shaderProgram->bind();
    
    shaderProgram->setUniformValue("model", transform.GetModelMatrix());
    if (mesh) mesh->Render(shaderProgram);
    
    shaderProgram->release();

    // Debug collider
    Rigidbody::Render(shaderProgram);
}

void Model::BuildAABB()
{
    if (mesh == nullptr) return;

    QVector3D minBounds = mesh->m_vertices[0].position;
    QVector3D maxBounds = mesh->m_vertices[0].position;

    for (const Vertex& vertex : mesh->m_vertices)
    {
        minBounds.setX(qMin(minBounds.x(), vertex.position.x()));
        minBounds.setY(qMin(minBounds.y(), vertex.position.y()));
        minBounds.setZ(qMin(minBounds.z(), vertex.position.z()));

        maxBounds.setX(qMax(maxBounds.x(), vertex.position.x()));
        maxBounds.setY(qMax(maxBounds.y(), vertex.position.y()));
        maxBounds.setZ(qMax(maxBounds.z(), vertex.position.z()));
    }
    

    bounds.position = (minBounds + maxBounds) * 0.5f;
    bounds.size = (maxBounds - minBounds) * 0.5f;

}

// void Model::SolveConstraints(const std::vector<std::shared_ptr<Rigidbody>>& constraints)
// {
    /*for (auto& rb : constraints) {
        if (rb.get() == this || !rb->HasVolume()) continue;

        if (type == RIGIDBODY_TYPE_SPHERE && rb->type == RIGIDBODY_TYPE_SPHERE) {
            SolveSphereSphereCollision(sphereCollider, rb->sphereCollider, rb.get());
        }
        else if (type == RIGIDBODY_TYPE_SPHERE && rb->type == RIGIDBODY_TYPE_BOX) {
            SolveSphereOBBCollision(sphereCollider, rb->boxCollider, rb.get());
        }
        else if (type == RIGIDBODY_TYPE_BOX && rb->type == RIGIDBODY_TYPE_SPHERE) {
            SolveSphereOBBCollision(rb->sphereCollider, boxCollider, this);
        }
        else if (type == RIGIDBODY_TYPE_BOX && rb->type == RIGIDBODY_TYPE_BOX) {
            SolveOBBOBBCollision(boxCollider, rb->boxCollider, rb.get());
        }
    }*/
// }
