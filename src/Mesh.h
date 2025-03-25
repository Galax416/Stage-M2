#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>

struct AABB 
{
	QVector3D position;
	QVector3D size; // HALF SIZE!

	inline AABB() : size(1, 1, 1) { }
	inline AABB(const QVector3D& p, const QVector3D& s) :
		position(p), size(s) { }
};

struct OBB 
{
	QVector3D position;
	QVector3D size; // HALF SIZE!
	QMatrix3x3 orientation; // euler angles

	inline OBB() : size(1, 1, 1) { }
	inline OBB(const QVector3D& p, const QVector3D& s) :
		position(p), size(s) { }
	inline OBB(const QVector3D& p, const QVector3D& s, const QMatrix3x3& o) :
		position(p), size(s), orientation(o) { }
};

struct BVHNode 
{
	AABB bounds;
	BVHNode* children;
	int numTriangles;
	int* triangles;

	BVHNode() : children(nullptr), numTriangles(0), triangles(nullptr) {}
};

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
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;

    std::vector<Triangle> m_triangles;
    BVHNode* accelerator;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Render(QOpenGLShaderProgram* shaderProgram);
    void BuildBVH();

private:
    QOpenGLVertexArrayObject *VAO;
    QOpenGLBuffer *VBO, *EBO;
    void SetUpMesh();
};