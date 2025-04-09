#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) :
    m_vertices(vertices), m_indices(indices), m_material(material),
    VAO(new QOpenGLVertexArrayObject), VBO(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)), IBO(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))
{
    initializeOpenGLFunctions();

    VAO->create();
    VBO->create();
    IBO->create();

	UnifySharedVertices();
    SetUpMesh();

}

Mesh::~Mesh()
{
    if (VAO) {
        VAO->destroy();
        delete VAO;
        VAO = nullptr;
    }
    if (VBO) {
        VBO->destroy();
        delete VBO;
        VBO = nullptr;
    }
    if (IBO) {
        IBO->destroy();
        delete IBO;
        IBO = nullptr;
    }

}

void Mesh::SetUpMesh()
{   
    VAO->bind();
    
    VBO->bind();
    VBO->allocate(m_vertices.data(), m_vertices.size() * sizeof(Vertex));

    IBO->bind();
    IBO->allocate(m_indices.data(), m_indices.size() * sizeof(unsigned int));
    
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

    // Send material properties to shader
    shaderProgram->setUniformValue("material.albedo", m_material.albedo);
    shaderProgram->setUniformValue("material.specular", m_material.specular);
    shaderProgram->setUniformValue("material.emissive", m_material.emissive);
    shaderProgram->setUniformValue("material.shininess", m_material.shininess);
    shaderProgram->setUniformValue("material.metalness", m_material.metalness);
    shaderProgram->setUniformValue("material.roughness", m_material.roughness);

    // Bind the VAO and draw the mesh
    VAO->bind();
    VBO->bind();
    IBO->bind();

    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

    VBO->release();
    IBO->release();
    VAO->release();

    shaderProgram->release();
}

void Mesh::ComputeNormals()
{

	// Reset normals to zero
	for (auto& vertex : m_vertices) {
		vertex.normal = QVector3D(0.0f, 0.0f, 0.0f);
	}

	// Compute normals for each triangle and accumulate them in the vertices	
	for (long unsigned int i = 0; i < m_indices.size(); i += 3) {
		unsigned int index1 = m_indices[i];
		unsigned int index2 = m_indices[i + 1];
		unsigned int index3 = m_indices[i + 2];

		QVector3D edge1 = m_vertices[index2].position - m_vertices[index1].position;
		QVector3D edge2 = m_vertices[index3].position - m_vertices[index1].position;
		QVector3D normal = QVector3D::crossProduct(edge1, edge2).normalized();

		m_vertices[index1].normal += normal;
		m_vertices[index2].normal += normal;
		m_vertices[index3].normal += normal;
	}

	for (auto& vertex : m_vertices) {
		vertex.normal.normalize();
	}
}

void Mesh::UnifySharedVertices()
{
	QHash<QVector3D, int> uniqueVertices;
    std::vector<Vertex> newVertices;
    std::vector<unsigned int> newIndices;

	for (long unsigned int i = 0; i < m_indices.size(); ++i) {
        const QVector3D& position = m_vertices[m_indices[i]].position;

        if (uniqueVertices.contains(position)) {
            newIndices.push_back(uniqueVertices[position]);
        } else {
            int newIndex = newVertices.size();
            uniqueVertices[position] = newIndex;

            newVertices.push_back(m_vertices[m_indices[i]]);
            newIndices.push_back(newIndex);
        }
    }

	m_vertices = newVertices;
    m_indices = newIndices;

	ComputeNormals();

}
