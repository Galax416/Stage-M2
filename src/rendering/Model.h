#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QString>
#include <QVector3D>
#include <QColor>
#include <QtMath>
// #include <QFileInfo>
#include <vector>

// #include "stb_image.h"
#include "Rigidbody.h"

#include <iostream>

// Forward declarations
class Mesh;
class CustomOBJLoader;

// Model class
class Model : public Rigidbody, protected QOpenGLFunctions
{
protected:
    void Init();
    void BuildAABB();
    void SetUpColliders();

public:
    Mesh* mesh = nullptr;
    CustomOBJLoader* customOBJ = nullptr;
    QColor color { 255, 255, 255 };
    AABB bounds;
    // bool flag;
    // Model *parent;

    Model();
    Model(const QString &path);
    virtual ~Model();

    void ClearModel();

    void LoadModel(const QString &path);
    void Update(float deltaTime);
    void Render(QOpenGLShaderProgram* shaderProgram);

    // void SolveConstraints(const std::vector<std::shared_ptr<Rigidbody>>& constraints);

    inline void SetPosition(const QVector3D& p) { transform.position = p; oldPosition = p; }

    void SynsCollisionVolumes();

};



