#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QThread>
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

#define DELTATIME  0.0160f
#define GRAVITY    QVector3D(0.0f, -9.81f, 0.0f)

#define Verbose false // Set to true to enable verbose output

// Foward declarations
class Camera;
class PhysicsWorker;
class Box;

enum ViewMode
{
    View1,
    View2,
    View3
};

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

    void SetViewMode(ViewMode mode);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void closeEvent(QCloseEvent *event) override;

    void mousePressEvent(QMouseEvent *event)   override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event)    override;
    void wheelEvent(QWheelEvent *event)        override;

    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;

signals:
    void statusBarMessageChanged(const QString& message);
    void buttonStateChanged(bool isPaused);
    void renderColliderChanged(bool render);
    void renderBVHChanged(bool render);
    void updateSpringsStiffnessControlsChanged(const std::vector<std::shared_ptr<Spring>>& springs);
    void update3DModelParametersChanged(VoxelGrid voxel);
    void physcisStateChanged(bool isPaused);
    void deltaTimeChanged(float deltaTime);
    void breastSlidersChanged();

public slots:
    void setGlobalDeltaTime(float value)        { Stop(); m_deltaTime = value; emit deltaTimeChanged(value); }
    void setGlobalFriction(float value)         { m_globalFriction = (1.0f - value); m_physicsSystem->ChangeFriction(m_globalFriction);}
    void setGlobalBackgroundColor(QColor color) { m_backgroundColor = color; if (Verbose) qDebug() << "setGlobalBackgroundColor"; Reset(); }

    void setCrossSpringModel(bool value) { m_crossSpringModel = value; if (Verbose) qDebug() << "setCrossSpringModel"; Reset(); }
    void setCurves(bool create)          { m_isCurve = create; if (Verbose) qDebug() << "setCurves"; Reset(); }
    void setVoxelModel(bool create)      { m_isVoxelModel = create; if (Verbose) qDebug() << "setVoxelModel"; Reset(); }
    void setThickness(bool value)        { m_haveThickness = value; if (Verbose) qDebug() << "setThickness"; Reset(); }
    void setSamplingModel(int value)     { m_numSamples  = value; if (Verbose) qDebug() << "setSamplingModel"; Reset(); }
    void setLayerModel(int value)        { m_curveLayers = value; if (Verbose) qDebug() << "setLayerModel"; Reset(); }
    void setDeformation(int p1, int p2, float value);
    void setCurveWidth(float value);
    void setCurveHeight(float value);
    void setCurveDepth(float value);
    void setCurveRing(float radius);

    // void Reset() { qDebug() << "-------------------"; makeCurrent(); Stop(); qDebug() << "Stop OK!"; ClearScene(); qDebug() << "Clear OK!"; doneCurrent(); CurveToParticlesSprings(); qDebug() << "CurveToParticle OK!"; VoxelToParticlesSprings(); qDebug() << "VoxelToParticle OK!"; InitScene(); qDebug() << "InitScene OK!"; }
    void Reset(); //{ makeCurrent(); Stop(); ClearScene(); doneCurrent(); CurveToParticlesSprings(); VoxelToParticlesSprings(); InitScene(); }
    void Stop()  { m_isRunning = false; emit buttonStateChanged(m_isRunning); }
    void Play()  { m_isRunning = true; m_renderBVH = false; emit renderBVHChanged(m_renderBVH); emit buttonStateChanged(m_isRunning); }
    
private:
    void InitShaders(QOpenGLShaderProgram *program, QString vertex_shader = "", QString geometry_shader = "", QString fragment_shader = "");
    
    void InitScene();
    void ClearScene() { m_physicsSystem->ClearAll(); m_particles.clear(); m_springs.clear(); m_triangleColliders.clear(); }
    
    void InitCurves();
    void UpdateCurveHeightWidth();
    void ChangeControlPointPosistion(const QVector3D& direction);
    void CurveToParticlesSprings();
    QVector3D EvaluateCurveSurface(float u, float v);
    QVector3D GetPointOntoMesh(const QVector3D& point);
    void FillVolumeWithParticle();
    std::vector<std::shared_ptr<TriangleCollider>> m_fillTriangleColliders;

    void InitVoxelModel();
    void VoxelToParticlesSprings();

    float m_deltaTime { DELTATIME };

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
    std::shared_ptr<Model> m_torsoModel;
    std::unique_ptr<BVHNode<TriangleCollider>> m_bvhTorsoColliders;
    Curve m_curve; // Cubic closed curve
    std::vector<QVector3D> m_curvePoints; // Control points of the curve
    std::vector<QVector3D> m_curvePointsSliders; 
    std::vector<QVector3D> m_profilePoints; // Sampled points of the curve
    std::vector<QVector3D> m_ringPoints; // Sampled points of the ring
    QVector3D m_curveNormal { 0.0f, 0.0f, 1.0f };
    bool  m_isCurve         { false };
    int   m_numSamples      { 32 };
    int   m_curveLayers     { 12 };
    float m_widthScale      { 0.94f };
    float m_heightScale     { 1.14f };
    float m_curveDepth      { 1.0f };
    float m_curveRingRadius { 0.1f };
    bool  m_haveThickness   { true };

    // Voxel model
    VoxelGrid m_voxel;
    bool m_isVoxelModel { false };
    std::shared_ptr<Box> m_press { nullptr };

    // Mode
    bool m_is2DMode { false };
    bool m_isRunning { false };
    bool m_isRunningTemp { false };
    bool m_isWireMode { false };

    // Colliders debug
    bool m_renderCollider { false }; // Render AABB of the colliders

    // BVH
    bool m_renderBVH { false };

    // Debug
    QVector3D breastDirection { 0.0f, 0.0f, 1.0f };

};