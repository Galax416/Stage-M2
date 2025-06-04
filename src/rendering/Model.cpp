#include "Model.h"
#include "Mesh.h"
#include "CustomOBJLoader.h"
#include "TriangleCollider.h"

Model::Model()
{
    Init();
}

Model::Model(const QString& path)
{
    Init();
    LoadModel(path);
}

void Model::ReleaseGLResources()
{
    if (mesh) mesh->ReleaseGLResources();
}

void Model::Init()
{
    initializeOpenGLFunctions();
    type = RIGIDBODY_TYPE_TRIANGLE;
    bounds = AABB();
    oldPosition = transform.position;
    mesh = std::make_unique<Mesh>();
    customOBJ = std::make_unique<CustomOBJLoader>();
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
    } else if (type == RIGIDBODY_TYPE_TRIANGLE) {
        for (size_t i = 0; i < mesh->indices.size(); i += 3) {
            QVector3D a = mesh->vertices[mesh->indices[i]].position;
            QVector3D b = mesh->vertices[mesh->indices[i + 1]].position;
            QVector3D c = mesh->vertices[mesh->indices[i + 2]].position;

            auto collider = std::make_shared<TriangleCollider>(a, b, c);
    
            triangleColliders.push_back(collider);
        }
    }
}


void Model::SetUpColliders()
{
    if (!mesh) return;

    BuildAABB();

    SynsCollisionVolumes();
}

void Model::LoadModel(const QString& path)
{

    // ResetModel();
    customOBJ->clear();

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

    mesh = std::make_unique<Mesh>(vertices, indices, material);
    
    SetUpColliders();
}

void Model::Update(float dt)
{

}

void Model::Render(QOpenGLShaderProgram* shaderProgram)
{
    shaderProgram->bind();
    
    shaderProgram->setUniformValue("material.albedo", QVector3D(color.redF(), color.greenF(), color.blueF()));
    shaderProgram->setUniformValue("transparency", static_cast<GLfloat>(color.alphaF()));
    shaderProgram->setUniformValue("model", transform.GetModelMatrix());
    
    if (mesh) mesh->Render(shaderProgram);
    
    shaderProgram->release();

    // Debug collider
    // Rigidbody::Render(shaderProgram);
}

void Model::BuildAABB()
{
    if (!mesh) return;
    if (mesh->vertices.empty()) return;

    QVector3D minBounds = mesh->vertices[0].position;
    QVector3D maxBounds = mesh->vertices[0].position;

    for (const Vertex& vertex : mesh->vertices)
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

void Model::SetPosition(const QVector3D& p) {
    transform.position = p;
    // oldPosition = p;

    SynsCollisionVolumes();
}

void Model::SetRotation(const QQuaternion& q) {
    transform.rotation = q;

    SynsCollisionVolumes();
}

bool Model::IsValid() const
{
    return mesh && !mesh->vertices.empty() && !mesh->indices.empty();
}
