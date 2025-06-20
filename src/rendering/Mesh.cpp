#include "Mesh.h"
#include "TriangleCollider.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) :
    vertices(vertices), indices(indices), material(material),
    VAO(std::make_unique<QOpenGLVertexArrayObject>()), VBO(std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer)), IBO(std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer))
{
    initializeOpenGLFunctions();

    VAO->create();
    VBO->create();
    IBO->create();

	UnifySharedVertices();
    SetUpMesh();
    // SetUpTriangles();

}

void Mesh::Clear() 
{
    vertices.clear();
    indices.clear();
    // triangles.clear();
    material = Material();
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

void Mesh::SetUpMesh()
{   
    VAO->bind();
    
    VBO->bind();
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

// void Mesh::SetUpTriangles()
// {
//     triangles.clear();
//     triangles.reserve(indices.size() / 3);

//     for (size_t i = 0; i < indices.size(); i += 3) {
//         Triangle triangle(
//             vertices[indices[i]].position,
//             vertices[indices[i + 1]].position,
//             vertices[indices[i + 2]].position
//         );
//         triangles.push_back(triangle);
//     }
// }

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

// std::shared_ptr<Mesh> Mesh::ToTetra(float spacing)
// {
    // std::vector<Vertex> vertices = this->vertices;
    // std::vector<unsigned int> indices = this->indices;

    // if (vertices.empty()) return nullptr;

    // // Bounding box
    // QVector3D minV = vertices[0].position;
    // QVector3D maxV = vertices[0].position;
    // for (const auto& v : vertices) {
    //     minV.setX(std::min(minV.x(), v.position.x()));
    //     minV.setY(std::min(minV.y(), v.position.y()));
    //     minV.setZ(std::min(minV.z(), v.position.z()));
    //     maxV.setX(std::max(maxV.x(), v.position.x()));
    //     maxV.setY(std::max(maxV.y(), v.position.y()));
    //     maxV.setZ(std::max(maxV.z(), v.position.z()));
    // }


    // QHash<QVector3D, int> pointIndex;
    // auto AddPoint = [&](const QVector3D& p) -> int {
    //     if (pointIndex.count(p)) return pointIndex[p];
    //     Vertex v;
    //     v.position = p;
    //     v.normal = QVector3D(0, 0, 1); // dummy
    //     v.texCoords = QVector2D(0, 0);
    //     v.tangent = QVector3D(1, 0, 0);
    //     v.bitangent = QVector3D(0, 1, 0);
    //     int idx = static_cast<int>(vertices.size());
    //     vertices.push_back(v);
    //     pointIndex[p] = idx;
    //     return idx;
    // };

    // for (float x = minV.x(); x < maxV.x(); x += spacing) {
    //     for (float y = minV.y(); y < maxV.y(); y += spacing) {
    //         for (float z = minV.z(); z < maxV.z(); z += spacing){
    //             QVector3D p0 = QVector3D(x, y, z);
    //             QVector3D p1 = p0 + QVector3D(spacing, 0, 0);
    //             QVector3D p2 = p0 + QVector3D(0, spacing, 0);
    //             QVector3D p3 = p0 + QVector3D(spacing, spacing, 0);
    //             QVector3D p4 = p0 + QVector3D(0, 0, spacing);
    //             QVector3D p5 = p0 + QVector3D(spacing, 0, spacing);
    //             QVector3D p6 = p0 + QVector3D(0, spacing, spacing);
    //             QVector3D p7 = p0 + QVector3D(spacing, spacing, spacing);
        
    //             std::array<std::array<QVector3D, 4>, 5> tetraList = {{
    //                 {p0, p1, p3, p6},
    //                 {p0, p3, p2, p6},
    //                 {p0, p5, p1, p6},
    //                 {p0, p4, p5, p6},
    //                 {p5, p7, p3, p6},
    //             }};
        
    //             for (const auto& tet : tetraList) {
    //                 QVector3D bary = (tet[0] + tet[1] + tet[2] + tet[3]) / 4.0f;
    //                 if (!IsPointInsideMesh(bary, triangles)) continue;
        
    //                 indices.push_back(AddPoint(tet[0]));
    //                 indices.push_back(AddPoint(tet[1]));
    //                 indices.push_back(AddPoint(tet[2]));
    //                 indices.push_back(AddPoint(tet[3]));
    //             }
    //         }
    //     }
    // }

    // return std::make_shared<Mesh>(vertices, indices, material);
// }
