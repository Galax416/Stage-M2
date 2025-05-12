#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QString>
#include <QVector3D>
#include <QColor>
#include <QtMath>
// #include <QFileInfo>
#include <vector>
#include <memory>

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
    QColor color { 255, 255, 255, 255 };

    void Init();
    void BuildAABB();
    void SetUpColliders();

public:
    std::shared_ptr<Mesh> mesh { nullptr };
    std::shared_ptr<CustomOBJLoader> customOBJ { nullptr };

    Model();
    Model(const QString &path);

    // void ResetModel();
    void LoadModel(const QString &path);

    void Update(float deltaTime) override;
    void Render(QOpenGLShaderProgram* shaderProgram) override;

    void SynsCollisionVolumes();

    void SetPosition(const QVector3D& p) override { transform.position = p; oldPosition = p; SynsCollisionVolumes(); }
    void SetRotation(const QQuaternion& q) override { transform.rotation = q; SynsCollisionVolumes(); }

    void SetColor(const QColor& c) { color = c; }
    bool IsValid() const;

private:
    AABB bounds;
};



