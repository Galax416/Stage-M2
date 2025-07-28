#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QString>
#include <QColor>
#include <QHash>
#include <vector>
#include <memory>

struct Vertex 
{
    QVector3D position;
    QVector3D normal;
    QVector2D texCoords;
    QVector3D tangent;
    QVector3D bitangent;
};

struct Texture 
{
    unsigned int id;
    QString type;
    QString path;
};

struct Material
{
    QVector3D albedo    {1.0f, 1.0f, 1.0f};
    QVector3D specular  {1.0f, 1.0f, 1.0f};
    QVector3D emissive  {0.0f, 0.0f, 0.0f};
    float shininess {32.0f};
    float metalness {0.0f};
    float roughness {0.0f};

    // std::vector<Texture> textures;
};

// Foward declaration 
// class CGALMesh;

class Mesh : protected QOpenGLFunctions 
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    Mesh() {}
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material);
    void ReleaseGLResources();
    void Render(QOpenGLShaderProgram* shaderProgram);
    void UpdateBuffers();
    void Clear();
    
    void ComputeNormals();

    // void FromCGALMesh();
    // void ToCGALMesh();

    // void Remesh(double targetLength);
    
private:
    // std::shared_ptr<CGALMesh> cgalMesh;
    std::unique_ptr<QOpenGLVertexArrayObject> VAO;
    std::unique_ptr<QOpenGLBuffer> VBO, IBO;

    void SetUpMesh();
    void UnifySharedVertices();
};