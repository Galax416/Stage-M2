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
    std::vector<std::shared_ptr<Particle>> particleRefs; 

    Model();
    Model(const QString &path);

    void ReleaseGLResources();
    
    void LoadModel(const QString &path);
    
    void Update(float dt) override;
    void Render(QOpenGLShaderProgram* shaderProgram) override;
    
    void SyncCollisionVolumes() override;

    AABB GetAABB() const override { return bounds; }
    
    void SetPosition(const QVector3D& p) override;
    void SetRotation(const QQuaternion& q) override;
    
    void SetColor(QColor c) { color = c; }
    QColor GetColor() const { return color; }
    bool IsValid() const;
    
    void ComputeFaces();
    // void Remesh(double targetLength);
    void SetDisplayAABB(bool display) { displayAABB = display; }

private:
    AABB bounds;
    bool displayAABB { false };

};



