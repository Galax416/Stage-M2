#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>

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
    std::string type;
    std::string path;
};

class Mesh : protected QOpenGLFunctions 
{
public:
    QVector<Vertex> m_vertices;
    QVector<unsigned int> m_indices;
    QVector<Texture> m_textures;

    // QVector<Triangle> m_triangles;
    // BVHNode* accelerator;

    Mesh(QVector<Vertex> vertices, QVector<unsigned int> indices, QVector<Texture> textures);
	~Mesh();
    void Render(QOpenGLShaderProgram* shaderProgram);
    void BuildBVH();

private:
    QOpenGLVertexArrayObject *VAO;
    QOpenGLBuffer *VBO, *IBO;

    void SetUpMesh();
};