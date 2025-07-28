#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaObject>
#include <vector>
#include <memory>
#include <set>
// #include <chrono>

#include "PhysicsSystem.h"
#include "PhysicsWorker.h"
#include "Curve.h"
#include "Voxel.h"
#include "ModelSettingsWidget.h"

// SCREEN SIZE
#define SCREEN_WIDTH  1080
#define SCREEN_HEIGHT 720

#define GRAVITY    QVector3D(0.0f, -9.81f, 0.0f)

#define Verbose true // Set to true to enable verbose output

// Foward declarations
class Camera;
class PhysicsWorker;
class Box;

enum ViewMode
{
    View1,
    View2,
    View3,
    View4,
    View5,
    View6
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
    void LoadScene(const QString& filename);
    void SaveScene(const QString& filename);

    void SetViewMode(ViewMode mode);
    void SetRenderWireframe() { m_isWireMode = !m_isWireMode; emit statusBarMessageChanged(QString("Wireframe mode: %1").arg(m_isWireMode ? "ON" : "OFF")); }
    void SetRenderCollider() { m_renderCollider = !m_renderCollider; emit renderColliderChanged(m_renderCollider); emit statusBarMessageChanged(QString("Render Collider: %1").arg(m_renderCollider ? "ON" : "OFF")); }
    void SetRenderBVH() { if (IsPaused()) { m_renderBVH = !m_renderBVH; emit renderBVHChanged(m_renderBVH); } emit statusBarMessageChanged(QString("Render BVH: %1").arg(m_renderBVH ? "ON" : "OFF")); }

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
    void setSizeSlider(int value);
    void setDeformationSlider(int p1, int p2, int value);
    void setBreastModel(bool value);
    void setSamplingModelSlider(int value);
    void setCurveSizeSlider(int value);
    void setCurveDepthSlider(int value);
    void setRingRadiusSlider(int value);  
    void setParticleRadiusVolumeSlider(int value);
    void setSpacingVolumeSlider(int value);
    void setAttachedCheckBox(bool value);
    void setAttachedToModelCheckBox(bool value);


public slots:
    void setGlobalDeltaTime(float value)        { Stop(); m_deltaTime = value; emit deltaTimeChanged(value); }
    void setGlobalFriction(float value)         { m_globalFriction = (1.0f - value); m_physicsSystem->ChangeFriction(m_globalFriction);}
    void setGlobalBackgroundColor(QColor color) { m_backgroundColor = color; if (Verbose) qDebug() << "setGlobalBackgroundColor" << m_backgroundColor; Reset(); }
    void gravityChanged();

    void setCrossSpringModel(bool value)      { m_crossSpringModel = value; if (Verbose) qDebug() << "setCrossSpringModel"; Reset(); }
    void setCurves(bool create)               { m_isCurve = create; m_isModel = false; m_isVoxelModel = false; SetDefaultCurveValues(); InitStiffnessValues(); if (Verbose) qDebug() << "setCurves"; Reset(); }
    void setVoxelModel(bool create)           { m_isVoxelModel = create; m_isModel = false; m_isCurve = false; InitStiffnessValues(); if (Verbose) qDebug() << "setVoxelModel"; Reset(); }
    void setThickness(bool value)             { m_haveThickness = value; InitStiffnessValues(); if (Verbose) qDebug() << "setThickness"; Reset(); }
    void setAttached(bool value)              { m_isAttached = value; InitStiffnessValues(); if (Verbose) qDebug() << "setAttached"; Reset(); }
    void setAttachedToModel(bool value)       { m_isAttachedToModel = value; InitStiffnessValues(); if (Verbose) qDebug() << "setAttachedToModel"; Reset(); }
    void addParticle();
    void setSamplingModel(int value)          { m_numSamples  = value; if (Verbose) qDebug() << "setSamplingModel" << m_numSamples; Reset(); }
    void setLayerModel(int value)             { m_curveLayers = value; if (Verbose) qDebug() << "setLayerModel" << m_curveLayers; Reset(); }
    void setParticleRadiusVolume(float value) { m_particleRadiusVolume = (4.5 * value * value + 9.5 * value + 6); if (Verbose) qDebug() << "setParticleRadiusVolume" << m_particleRadiusVolume; Reset(); } // f(-1) = 1.0, f(0) = 6.0, f(1) = 20.0,
    void setSpacingVolume(float value)        { m_spacingVolume = (0.105 * value * value + -0.175 * value + 0.12); if (Verbose) qDebug() << "setSpacingVolume" << m_spacingVolume; Reset(); } // f(-1) = 0.04, f(0) = 0.12, f(1) = 0.05
    void setDeformation(int p1, int p2, float value) { for (auto& entry : m_curveDeformation) { if (std::get<0>(entry) == p1 && std::get<1>(entry) == p2) std::get<2>(entry) = value; } UpdateCurveForm(p1, p2, value); if (Verbose) qDebug() << "setDeformation" << p1 << p2 << value; Reset(); }
    void setCurveWidth(float value)                  { m_widthScale = (value + 1.0f); UpdateCurveHeightWidth(); if (Verbose) qDebug() << "setCurveWidth"; Reset(); }
    void setCurveHeight(float value)                 { m_heightScale = (value + 1.0f); UpdateCurveHeightWidth(); if (Verbose) qDebug() << "setCurveHeight"; Reset(); }
    void setCurveSize(float value)                   { m_curveSize = (value + 1.0f); UpdateCurveHeightWidth(); if (Verbose) qDebug() << "setCurveSize" << m_curveSize; Reset(); }
    void setCurveDepth(float value)                  { m_curveDepth = (value + 1.0f); if (Verbose) qDebug() << "setCurveDepth" << m_curveDepth; Reset(); }
    void setCurveRing(float value)                   { m_curveRingRadius = (value + 1.0f) * 0.18f; if (Verbose) qDebug() << "setCurveRing" << m_curveRingRadius; Reset(); }
    void setNSegements(int value)                    { m_nSegments = value; m_angularWeights = std::vector<float>(value, 1.0f / static_cast<float>(value)); if (Verbose) qDebug() << "setNSegements" << m_nSegments; Reset(); }
    void setSegmentSliders(std::vector<float>& values){ if (m_angularWeights.size() != values.size()) return; m_angularWeights = values; if (Verbose) qDebug() << "setSegmentSliders"; Reset(); }
    void setNewStiffness(const int id, const float value) { auto it = m_stiffness.find(id); if (it != m_stiffness.end()) *(it->second) = value; }

    void ClearSceneSlot() { Stop(); ClearScene(); m_isModel = false; m_isCurve = false; m_isVoxelModel = false; m_renderBVH = false; m_isWireMode = false; Reset(); }
    void Reset();
    void Stop()  { m_isRunning = false; emit buttonStateChanged(m_isRunning); if (m_worker && m_physicsThread) QMetaObject::invokeMethod(m_worker, "Stop", Qt::BlockingQueuedConnection); }
    void Play()  { m_isRunning = true; m_renderBVH = false; emit renderBVHChanged(m_renderBVH); emit buttonStateChanged(m_isRunning); if (m_worker && m_physicsThread) QMetaObject::invokeMethod(m_worker, "SetPhysicsRunning", Qt::BlockingQueuedConnection, Q_ARG(bool, m_isRunning)); }
    
private:
    void InitShaders(QOpenGLShaderProgram *program, QString vertex_shader = "", QString geometry_shader = "", QString fragment_shader = "");
    
    void InitScene();
    void ClearScene() { makeCurrent(); m_physicsSystem->ClearAll(); m_particles.clear(); m_springs.clear(); m_triangleColliders.clear(); doneCurrent(); }
    
    void SetDefaultCurveValues() 
    {
        m_numSamples      = DEFAULT_SAMPLING;
        m_curveLayers     = DEFAULT_LAYERS;
        m_curveSize       = DEFAULT_SIZE / 50.0f;
        m_curveDepth      = DEFAULT_DEPTH / 50.0f;
        m_curveRingRadius = 0.18f;
        m_spacingVolume   = 0.12f;
        m_particleRadiusVolume = 6.0f; // Default particle radius for volume filling
        m_stiffnessAttached = 100.0f; // Default stiffness for attached particles
        m_angularWeights = std::vector<float>(DEFAULT_N_SEGMENTS, 1.0f / static_cast<float>(DEFAULT_N_SEGMENTS));
        m_stiffnessAreola = 1000.0f;
        m_stiffnessAttached = 100.0f; 
        m_nSegments = DEFAULT_N_SEGMENTS;
        m_haveThickness = true;
        m_stiffnessInterLayer = 500.0f;

        m_stiffnessBySegment.clear();
        switch ( m_nSegments )
        {
            case 1:
                m_stiffnessBySegment[0] = 400.0f;
                break;
            case 2:
                m_stiffnessBySegment[0] = 400.0f;
                m_stiffnessBySegment[1] = 410.0f;
                break;
            case 3:
                m_stiffnessBySegment[0] = 400.0f;
                m_stiffnessBySegment[1] = 450.0f;
                m_stiffnessBySegment[2] = 410.0f;
                break;
            case 4:
                m_stiffnessBySegment[0] = 400.0f;
                m_stiffnessBySegment[1] = 450.0f;
                m_stiffnessBySegment[2] = 410.0f;
                m_stiffnessBySegment[3] = 4550.0f;
                break;
        }
        // InitCurves();

    }

    void InitCurves();
    void UpdateCurveForm(int i1, int i2, float value);
    void UpdateCurveHeightWidth();
    void ChangeControlPointPosistion(const QVector3D& direction);
    void CurveToParticlesSprings();
    void BuildBreast(const std::vector<QVector3D>& profile);
    bool GetPointOntoMesh(QVector3D& point);
    void FillVolumeWithParticle(const std::vector<QVector3D>& profile, std::vector<std::shared_ptr<TriangleCollider>>& triangleColliders);
    void Add2ndLayer(std::vector<std::shared_ptr<Particle>>& particles, std::vector<std::shared_ptr<Spring>>& springs, const QVector3D& center, const float thickness, const float stiffnessInterLayer);
    std::vector<std::shared_ptr<TriangleCollider>> m_fillTriangleColliders;

    void InitVoxelModel();
    void VoxelToParticlesSprings();

    void InitStiffnessValues();

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
    std::unique_ptr<BVHNode<TriangleCollider>> m_bvhSceneCollider;

    // Model settings
    std::unordered_map<int, float*> m_stiffness;
    bool m_isModel { false }; // Is a model loaded
    bool m_crossSpringModel { true }; // Cross spring model

    // Custom Parametric Model
    std::shared_ptr<Model> m_torsoModel;
    std::unique_ptr<BVHNode<TriangleCollider>> m_bvhTorsoColliders;
    Curve m_curve; // Cubic closed curve
    std::vector<QVector3D> m_initialCurvePoints;
    std::vector<QVector3D> m_curvePoints; // Control points of the curve
    std::vector<QVector3D> m_curvePointsSliders; 
    std::vector<QVector3D> m_defaultCurvePoints;
    std::vector<QVector3D> m_profilePoints; // Sampled points of the curve
    std::vector<std::tuple<int, int, float>> m_curveDeformation;
    std::tuple<int, int, float> m_lastValidDeformation; // Last valid deformation values
    QVector3D m_curveCenter      { 0.0f, 0.0f, 0.0f }, m_initialCurveCenter;
    QVector3D m_curveNormal      { 0.0f, 0.0f, 1.0f };
    bool  m_isCurve              { false };
    int   m_numSamples           { DEFAULT_SAMPLING };
    int   m_curveLayers          { DEFAULT_LAYERS };
    float m_widthScale           { 1.0f/* 0.94f */ };
    float m_heightScale          { 1.0f/* 1.14f */ };
    float m_curveSize            { DEFAULT_SIZE / 50.0f };
    float m_lastValidCurveSize   { DEFAULT_SIZE / 50.0f }; // Last valid curve size
    float m_curveDepth           { DEFAULT_DEPTH / 50.0f };
    float m_curveRingRadius      { 0.18f };
    bool  m_haveThickness        { true };
    float m_stiffnessInterLayer  { 500.0f }; // Stiffness for inter-layer particles
    bool  m_isAttached           { false }; 
    bool  m_isAttachedToModel    { false }; 
    float m_spacingVolume        { 0.12f };
    float m_particleRadiusVolume { 6.0f }; 
    float m_stiffnessAreola      { 1000.0f }; // Stiffness for areola particles
    float m_stiffnessAttached    { 100.0f }; // Stiffness for attached particles
    int m_nSegments              { DEFAULT_N_SEGMENTS };
    std::unordered_map<int, float> m_stiffnessBySegment = std::unordered_map<int, float>(DEFAULT_N_SEGMENTS);
    std::vector<float> m_angularWeights = std::vector<float>(DEFAULT_N_SEGMENTS, 1.0f / static_cast<float>(DEFAULT_N_SEGMENTS));

    // Voxel model
    VoxelGrid m_voxel;
    bool m_isVoxelModel { false };
    std::shared_ptr<Box> m_press { nullptr };

    // Get distance between two points
    std::vector<std::shared_ptr<Particle>> m_distPoints;

    // Mode
    bool m_is2DMode   { false };
    bool m_isRunning  { false };
    bool m_isWireMode { false };

    // Colliders debug
    bool m_renderCollider { false }; // Render AABB of the colliders

    // BVH
    bool m_renderBVH { false };

    // Debug
    // QVector3D breastDirection { 0.0f, 0.0f, 1.0f };
    // Ray m_debugRay { QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f) };

};