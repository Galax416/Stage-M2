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

inline uint qHash(const QVector3D& key, uint seed = 0) 
{
    return qHash(key.x(), seed) ^ qHash(key.y(), seed << 1) ^ qHash(key.z(), seed << 2);
}

struct Vertex 
{
    QVector3D position;
    QVector3D normal;
    QVector2D texCoords;
    QVector3D tangent;
    QVector3D bitangent;
};

struct Triangle 
{
	QVector3D a;
	QVector3D b;
	QVector3D c;

	inline Triangle() {}
	inline Triangle(const QVector3D& a, const QVector3D& b, const QVector3D& c) :
		a(a), b(b), c(c) { }
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

class Mesh : protected QOpenGLFunctions 
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    // std::vector<Triangle> triangles;
    Material material;

    Mesh() {}
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material);
    void ReleaseGLResources();

    void Render(QOpenGLShaderProgram* shaderProgram);

    void Clear();
    // std::shared_ptr<Mesh> ToTetra(float spacing = 0.1f);
    
private:
    std::unique_ptr<QOpenGLVertexArrayObject> VAO;
    std::unique_ptr<QOpenGLBuffer> VBO, IBO;

    void SetUpMesh();
    // void SetUpTriangles();
    void ComputeNormals();
    void UnifySharedVertices();
};