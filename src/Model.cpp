#include "Model.h"

Model::Model()
{
    initializeOpenGLFunctions();
    rigidbody.type = RIGIDBODY_TYPE_BOX;
}

Model::Model(const QString& path)
{
    initializeOpenGLFunctions();
    rigidbody.type = RIGIDBODY_TYPE_BOX;
    // LoadModel(path);
}

Model::~Model()
{
    
}

void Model::Render(QOpenGLShaderProgram* shaderProgram)
{
    shaderProgram->bind();
    
    for (auto& mesh : meshes) {
        shaderProgram->setUniformValue("model", rigidbody.transform.GetModelMatrix()); // use rigidbody transform instead !!!
        shaderProgram->setUniformValue("color", color);
        mesh->Render(shaderProgram);
    }

    shaderProgram->release();
}