#include "Mesh.h"

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
