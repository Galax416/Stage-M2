#include "Mesh.h"
#include "TriangleCollider.h"
#include "Utils.h"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>

namespace PMP = CGAL::Polygon_mesh_processing;
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> SurfaceMesh;


// CGALMesh is a wrapper around CGAL::Surface_mesh<Point>
class CGALMesh {
public:
    SurfaceMesh mesh;

    void FromVertices(const std::vector<Vertex>& verts, const std::vector<unsigned int>& indices) {
        mesh.clear();
        std::vector<SurfaceMesh::Vertex_index> vmap;
        for (const auto& v : verts)
            vmap.push_back(mesh.add_vertex(Point(v.position.x(), v.position.y(), v.position.z())));

        for (size_t i = 0; i + 2 < indices.size(); i += 3)
            mesh.add_face(vmap[indices[i]], vmap[indices[i+1]], vmap[indices[i+2]]);
    }

    void ToVertices(std::vector<Vertex>& verts, std::vector<unsigned int>& indices) {
        verts.clear();
        indices.clear();
        std::unordered_map<SurfaceMesh::Vertex_index, int> vmap;
        int idx = 0;

        for (auto v : mesh.vertices()) {
            const auto& p = mesh.point(v);
            Vertex vert;
            vert.position = QVector3D(p.x(), p.y(), p.z());
            vert.normal = QVector3D(0, 0, 0); // Normals will be computed later
            vert.texCoords = QVector2D(0, 0);
            vert.tangent = QVector3D(1, 0, 0);
            vert.bitangent = QVector3D(0, 1, 0);
            verts.push_back(vert);
            vmap[v] = idx++;
        }

        for (auto f : mesh.faces()) {
            std::vector<unsigned int> face;
            for (auto v : CGAL::vertices_around_face(mesh.halfedge(f), mesh))
                face.push_back(vmap[v]);

            if (face.size() == 3)
                indices.insert(indices.end(), face.begin(), face.end());
        }
    }

    void Remesh(double targetLength) {
        PMP::isotropic_remeshing(
            faces(mesh),
            targetLength,
            mesh,
            PMP::parameters::number_of_iterations(3) // .protect_constraints(true)
        );
    }
};

// Mesh class implementation
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) :
    vertices(vertices), indices(indices), material(material), cgalMesh(std::make_shared<CGALMesh>()),
    VAO(std::make_unique<QOpenGLVertexArrayObject>()), VBO(std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer)), IBO(std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer))
{
    initializeOpenGLFunctions();

    VAO->create();
    VBO->create();
    IBO->create();

	// UnifySharedVertices();
    ComputeNormals();
    SetUpMesh();

}

void Mesh::ReleaseGLResources() 
{
    if (QOpenGLContext::currentContext() == nullptr) {
        qWarning() << "releaseGLResources() called without valid OpenGL context!";
        return;
    }
    if (VAO && VAO->isCreated()) VAO->destroy();
    if (VBO && VBO->isCreated()) VBO->destroy();
    if (IBO && IBO->isCreated()) IBO->destroy();
}

void Mesh::Clear() 
{
    vertices.clear();
    indices.clear();
}

void Mesh::FromCGALMesh()
{
    cgalMesh->ToVertices(vertices, indices);

    ComputeNormals();
    SetUpMesh();
}

void Mesh::ToCGALMesh()
{
    cgalMesh->FromVertices(vertices, indices);

    // Ensure the mesh is triangulated
    PMP::triangulate_faces(cgalMesh->mesh);
}

void Mesh::Remesh(double target_length)
{
    ToCGALMesh();
    cgalMesh->Remesh(target_length);
    FromCGALMesh();
}

void Mesh::SetUpMesh()
{   
    VAO->bind();
    
    VBO->bind();
    VBO->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    VBO->allocate(vertices.data(), vertices.size() * sizeof(Vertex));

    IBO->bind();
    IBO->allocate(indices.data(), indices.size() * sizeof(unsigned int));
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); // Normal
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)); // TexCoords
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent)); // Tangent
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent)); // Bitangent
    
    VBO->release();
    IBO->release();
    VAO->release();
}

void Mesh::UpdateBuffers()
{
    if (!VBO || !VBO->isCreated()) return;
    
    VBO->bind();
    VBO->write(0, vertices.data(), vertices.size() * sizeof(Vertex));
    VBO->release();
}

void Mesh::Render(QOpenGLShaderProgram* shaderProgram)
{
    shaderProgram->bind();

    // // Send material properties to shader
    // shaderProgram->setUniformValue("material.albedo", m_material.albedo);
    shaderProgram->setUniformValue("material.specular", material.specular);
    shaderProgram->setUniformValue("material.emissive", material.emissive);
    shaderProgram->setUniformValue("material.shininess", material.shininess);
    shaderProgram->setUniformValue("material.metalness", material.metalness);
    shaderProgram->setUniformValue("material.roughness", material.roughness);

    // Bind the VAO and draw the mesh
    VAO->bind();
    VBO->bind();
    IBO->bind();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    VBO->release();
    IBO->release();
    VAO->release();

    shaderProgram->release();
}

void Mesh::ComputeNormals()
{
	// Reset normals to zero
	for (auto& vertex : vertices) {
		vertex.normal = QVector3D(0.0f, 0.0f, 0.0f);
	}

	// Compute normals for each triangle and accumulate them in the vertices	
	for (long unsigned int i = 0; i < indices.size(); i += 3) {
		unsigned int index1 = indices[i];
		unsigned int index2 = indices[i + 1];
		unsigned int index3 = indices[i + 2];

		QVector3D edge1 = vertices[index2].position - vertices[index1].position;
		QVector3D edge2 = vertices[index3].position - vertices[index1].position;
		QVector3D normal = QVector3D::crossProduct(edge1, edge2).normalized();

		vertices[index1].normal += normal;
		vertices[index2].normal += normal;
		vertices[index3].normal += normal;
	}

	for (auto& vertex : vertices) {
		vertex.normal.normalize();
	}
}

void Mesh::UnifySharedVertices()
{
	QHash<QVector3D, int> uniqueVertices;
    std::vector<Vertex> newVertices;
    std::vector<unsigned int> newIndices;

	for (long unsigned int i = 0; i < indices.size(); ++i) {
        const QVector3D& position = vertices[indices[i]].position;

        if (uniqueVertices.contains(position)) {
            newIndices.push_back(uniqueVertices[position]);
        } else {
            int newIndex = newVertices.size();
            uniqueVertices[position] = newIndex;

            newVertices.push_back(vertices[indices[i]]);
            newIndices.push_back(newIndex);
        }
    }

	vertices = newVertices;
    indices = newIndices;

	ComputeNormals();
}
