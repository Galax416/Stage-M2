#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QThread>
#include <QMutex>
#include <QMutexLocker> // For thread safety
#include <vector>
#include <memory>
#include <set>
// #include <chrono>

#include "PhysicsSystem.h"
// #include "PhysicsWorker.h"
#include "Curve.h"
#include "Voxel.h"

// SCREEN SIZE
#define SCREEN_WIDTH  1080
#define SCREEN_HEIGHT 720

#define DeltaTime  0.0016f

// Foward declarations
class Camera;
class PhysicsWorker;
class Box;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget() override;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    bool IsPaused()  const { return !m_isRunning; }
    bool IsRunning() const { return  m_isRunning; }  
    void LoadOBJ(const QString& filename);
    void SaveOBJ(const QString& filename);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    // void closeEvent(QCloseEvent *event) override;

    void mousePressEvent(QMouseEvent *event)   override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event)    override;
    void wheelEvent(QWheelEvent *event)        override;

    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;

signals:
    void statusBarMessageChanged(const QString& message);
    void buttonStateChanged(bool isPaused);
    void renderBVHChanged(bool render);
    void updateSpringsStiffnessControlsChanged(const std::vector<std::shared_ptr<Spring>>& springs);
    void update3DModelParametersChanged(VoxelGrid voxel);
    void physcisStateChanged(bool isPaused);
    void deltaTimeChanged(float deltaTime);

public slots:
    void setGlobalDeltaTime(float value)        { m_deltaTime = value; emit deltaTimeChanged(value); }
    void setGlobalFriction(float value)         { m_globalFriction = (1.0f - value); m_physicsSystem->ChangeFriction(m_globalFriction);}
    void setGlobalBackgroundColor(QColor color) { m_backgroundColor = color; Reset(); }
    void setGlobalRotation(QVector3D rotation)  { m_globalRotation = rotation; Reset(); }

    void setCrossSpringModel(bool value) { m_crossSpringModel = value; Reset(); }
    void setCurves(bool create)          { m_isCurve = create; if (create) InitCurves(); else Reset(); }
    void setVoxelModel(bool create)      { m_isVoxelModel = create; if (create) InitVoxelModel(); else Reset(); }
    void setThickness(bool value)        { m_haveThickness = value; Reset(); }
    void setSamplingModel(int value)     { m_numSamples  = value; Reset(); }
    void setLayerModel(int value)        { m_curveLayers = value; Reset(); }
    void setDeformation(int p1, int p2, float value);
    void setHeight(float value);
    void setRing(float radius);

    // void Clear() { ClearScene(); /* InitScene(); */ }
    void Reset() { makeCurrent(); Stop(); ClearScene(); doneCurrent(); CurveToParticlesSprings(); VoxelToParticlesSprings(); InitScene(); }
    void Stop()  { m_isRunning = false ; emit buttonStateChanged(m_isRunning); }
    void Play()  { m_isRunning = true  ; m_renderBVH = false; emit renderBVHChanged(m_renderBVH); emit buttonStateChanged(m_isRunning); }
    
private:
    void InitShaders(QOpenGLShaderProgram *program, QString vertex_shader = "", QString geometry_shader = "", QString fragment_shader = "");
    
    void InitScene();
    void ClearScene() { m_physicsSystem->ClearAll(); m_particles.clear(); m_springs.clear(); m_triangleColliders.clear(); }
    
    void InitCurves();
    void CurveToParticlesSprings();
    std::vector<std::shared_ptr<TriangleCollider>> m_fillTriangleColliders;
    void FillVolumeWithParticle();

    void InitVoxelModel();
    void VoxelToParticlesSprings();

    float m_deltaTime { DeltaTime };

    std::shared_ptr<QOpenGLShaderProgram> m_program;
    std::shared_ptr<QOpenGLShaderProgram> m_program2D, m_program3D;

    std::shared_ptr<Camera> m_camera;

    std::shared_ptr<PhysicsSystem> m_physicsSystem;
    PhysicsWorker* m_worker { nullptr };
    std::unique_ptr<QThread> m_physicsThread;

    std::vector<std::shared_ptr<Particle>>         m_particles;
    std::vector<std::shared_ptr<Spring>>           m_springs; 
    std::vector<std::shared_ptr<TriangleCollider>> m_triangleColliders;
    std::shared_ptr<Model> m_model; // To load model from file

    // Global settings
    float     m_globalFriction  { 0.95f };
    QColor    m_backgroundColor { 25, 25, 25, 255 };
    QVector3D m_globalRotation  { 0.0f, 0.0f, 0.0f };

    // Model settings
    bool m_crossSpringModel { true }; // Cross spring model

    // Custom Parametric Model
    Curve m_curve; // Cubic closed curve
    std::vector<QVector3D> m_curvePoints; // Control points of the curve
    bool  m_isCurve         { false };
    int   m_numSamples      { 32 };
    int   m_curveLayers     { 5 };
    float m_curveHeight     { 1.0f };
    float m_curveRingRadius { 0.1f };
    bool  m_haveThickness   { false };

    // Voxel model
    VoxelGrid m_voxel;
    bool m_isVoxelModel { false };
    std::shared_ptr<Box> m_press { nullptr };

    // Mode
    bool m_is2DMode   { false };
    bool m_isRunning  { false };
    bool m_isWireMode { false };

    // BVH
    bool m_renderBVH { false };

};