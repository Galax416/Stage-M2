#pragma once

#include <QOpenGLShaderProgram>
#include <QString>
#include <QVector3D>
#include <QVector>
#include <QColor>
#include <QtMath>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Rigidbody.h"
#include "Mesh.h"

class Model : protected QOpenGLFunctions
{
public:
    Rigidbody rigidbody;
    QVector<Mesh*> meshes;
    QColor color;
    // AABB bounds;
    // bool flag;
    // Model *parent;

    Model();   
    Model(const QString &path);
    ~Model();

    // void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);
    // void ApplyForces();
    // void SolveConstraints(const QVector<Rigidbody*>& constraints);

    // void BuildBVH();

    // AABB GetBounds() const { return bounds; }

private:
    QString m_directory;
    QVector<Texture> m_textures_loaded;

    void LoadModel(const QString &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    void ProcessMesh(aiMesh *mesh, const aiScene *scene);
    QVector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName);
    uint TextureFromFile(const char *path, const QString &directory);
};

