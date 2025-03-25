#pragma once

#include <QOpenGLShaderProgram>
#include <QString>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Transform.h"
#include "Mesh.h"

class Model : public Transform
{
public:
    std::vector<Mesh> m_meshes;
    AABB bounds;
    bool flag;
    Model *parent;

    Model() {};
    Model(const QString &path);
    void Render(QOpenGLShaderProgram* shaderProgram);
    void BuildBVH();

private:
    QString m_directory;
    std::vector<Texture> m_textures_loaded;

    void LoadModel(const QString &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    void ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName);
    uint TextureFromFile(const char *path, const QString &directory);
};