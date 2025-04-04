#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QString>
#include <QColor>
#include <QHash>

#include "Utils.h"
#include "Transform.h"
#include "Collision.h"

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
    QVector3D albedo;
    QVector3D specular;
    QVector3D emissive;
    float shininess = 32.0f;
    float metalness = 0.0f;
    float roughness = 0.0f;

    std::vector<Texture> textures;
};

class Mesh : protected QOpenGLFunctions 
{
public:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    Material m_material;

    // std::vector<Triangle> m_triangles;
    // BVHNode* accelerator;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material);
	~Mesh();
    void Render(QOpenGLShaderProgram* shaderProgram);
    // void BuildBVH();
    

private:
    QOpenGLVertexArrayObject *VAO;
    QOpenGLBuffer *VBO, *IBO;

    void SetUpMesh();
    // void ComputeTriangleList();
    void ComputeNormals();
    void UnifySharedVertices();
};

// void AccelerateMesh(Mesh& mesh);
// void SplitBVHNode(BVHNode* node, const Mesh& model, int depth);
// void FreeBVHNode(BVHNode* node);

// bool OverlapOnAxis(const AABB& aabb, const Triangle& triangle, const QVector3D& axis);
// bool TriangleAABB(const Triangle& t, const AABB& aabb);

// Interval GetInterval(const AABB& aabb, const QVector3D& axis);
// Interval GetInterval(const OBB& obb, const QVector3D& axis);
// Interval GetInterval(const Triangle& triangle, const QVector3D& axis);

// float MeshRay(const Mesh& mesh, const Ray& ray);
// bool MeshAABB(const Mesh& mesh, const AABB& aabb);