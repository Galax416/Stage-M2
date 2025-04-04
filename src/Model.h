#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QString>
#include <QVector3D>
#include <QColor>
#include <QtMath>
#include <QFileInfo>
#include <vector>

// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>

#include "Rigidbody.h"
#include "Mesh.h"
#include "CustomOBJLoader.h"
// #include "stb_image.h"

#include <iostream>

class Model : public Rigidbody, protected QOpenGLFunctions
{
public:
    Mesh* mesh = nullptr;
    CustomOBJLoader* customOBJ = nullptr;
    QColor color;
    AABB bounds;
    // bool flag;
    // Model *parent;

    Model(); 
    Model(const QString &path);
    ~Model();

    void Init();
    void SetUpColliders();

    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);
    // void ApplyForces();
    void SolveConstraints(const std::vector<Rigidbody*>& constraints);

    void SetPosition(const QVector3D& p) { transform.position = p; oldPosition = p; }

    void SynsCollisionVolumes();

    void BuildAABB();
    // void BuildBVH();

    

private:
    // QString m_directory;
    // std::vector<Texture> m_textures_loaded;

    void LoadModel(const QString &path);
    // void ProcessNode(aiNode *node, const aiScene *scene);
    // Mesh* ProcessMesh(aiMesh *mesh, const aiScene *scene);
    // std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName);
    // unsigned int TextureFromFile(QString &path, const QString &directory);
};



