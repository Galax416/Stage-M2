#include "Mesh.h"

Mesh::Mesh(QVector<Vertex> vertices, QVector<unsigned int> indices, QVector<Texture> textures) :
    m_vertices(vertices), m_indices(indices), m_textures(textures),
    VAO(new QOpenGLVertexArrayObject), VBO(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)), IBO(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))
{
    initializeOpenGLFunctions();

    VAO->create();
    VBO->create();
    IBO->create();

    SetUpMesh();

}

Mesh::~Mesh()
{
    if (VAO) VAO->destroy();
    if (VBO) VBO->destroy();
    if (IBO) IBO->destroy();
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
    #if 0 // For debugging
    if (!shaderProgram || !shaderProgram->isLinked()) {
        qDebug() << "Shader program is null or not linked!";
        return;
    }
    int notCreatedCount = 0;
    if (!VAO || !VAO->isCreated()) {
        qDebug() << "VAO is not properly created!";
        notCreatedCount++;
    }
    if (!VBO || !VBO->isCreated()) {
        qDebug() << "VBO is not properly created!";
        notCreatedCount++;
    }
    if (!IBO || !IBO->isCreated()) {
        qDebug() << "IBO is not properly created!";
        notCreatedCount++;
    }
    if (notCreatedCount == 3) {
        qDebug() << "All VAO, VBO, and IBO are not properly created!";
        return;
    }
    if (m_vertices.empty()) {
        qDebug() << "Error: m_vertices is empty before VBO allocation!";
        return;
    }
    if (m_indices.empty()) {
        qDebug() << "Error: m_indices is empty before rendering!";
        return;
    }
    for (int index : m_indices) {
        if (index >= m_vertices.size()) {
            qDebug() << "Error: Index out of bounds! index=" << index << " max=" << m_vertices.size();
            return;
        }
    }
    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qDebug() << "Error: No active OpenGL context!";
        return;
    }
    #endif
    shaderProgram->bind();

    VAO->bind();
    VBO->bind();
    IBO->bind();
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    VBO->release();
    IBO->release();
    VAO->release();

    shaderProgram->release();
}