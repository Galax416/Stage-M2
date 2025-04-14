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
    QColor color { 255, 255, 255 };

    void Init();
    void BuildAABB();
    void SetUpColliders();

public:
    Mesh* mesh { nullptr };
    CustomOBJLoader* customOBJ { nullptr };

    Model();
    Model(const QString &path);
    virtual ~Model();

    void ClearModel();
    void LoadModel(const QString &path);

    void Update(float deltaTime) override;
    void Render(QOpenGLShaderProgram* shaderProgram) override;

    void SynsCollisionVolumes();

    void SetColor(const QColor& c) { color = c; }

private:
    AABB bounds;
};



