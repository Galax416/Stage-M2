#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <vector>
#include <memory>
// #include <chrono>

#include "PhysicsSystem.h"
#include "BezierCurve.h"

// SCREEN SIZE
#define SCREEN_WIDTH  1080
#define SCREEN_HEIGHT 720

#define DeltaTime  0.0016f 

// Foward declarations
class Camera;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    bool IsPaused() const { return m_isPaused; }
    void LoadOBJ(const QString& filename);
    void SaveOBJ(const QString& filename);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

signals:
    void statusBarMessageChanged(const QString& message);
    void buttonStateChanged(bool isPaused);
    void renderBVHChanged();
    void updateSpringsStiffnessControlsChanged(const std::vector<std::shared_ptr<Spring>>& springs);

public slots:
    void setGlobalDeltaTime(float value) { m_deltaTime = value; }
    void setGlobalFriction(float value) { m_globalFriction = (1.0f - value); m_physicsSystem.ChangeFriction(m_globalFriction);}
    void setGlobalRotation(QVector3D rotation) { m_globalRotation = rotation; Stop(); m_physicsSystem.RotateRigidbodies(m_globalRotation); makeCurrent(); update(); doneCurrent(); }

    void setCrossSpringModel(bool value) { m_crossSpringModel = value; Reset(); }
    void setCurves(bool create) { m_isCurve = create; if (create) InitCurves(); Reset(); }
    void setSamplingModel(int value) { m_numSamples = value; Reset(); }
    void setDeformation(int p1, int p2, float value);

    // void Clear() { ClearScene(); /* InitScene(); */ }
    void Reset() { ClearScene(); CurveToParticlesSprings(); InitScene(); }
    void Stop()  { m_isPaused = true ; emit buttonStateChanged(m_isPaused); }
    void Play()  { m_isPaused = false; emit buttonStateChanged(m_isPaused); }
    

private:
    void InitShaders(QOpenGLShaderProgram *program, QString vertex_shader = "", QString geometry_shader = "", QString fragment_shader = "");
    void InitScene();
    void ClearScene() { m_physicsSystem.ClearAll(); m_particles.clear(); m_springs.clear(); m_triangleColliders.clear(); }
    
    void InitCurves();
    void CurveToParticlesSprings();
    // QOpenGLVertexArrayObject* m_vao;

    float m_deltaTime = DeltaTime;

    QOpenGLShaderProgram *m_program;
    std::shared_ptr<QOpenGLShaderProgram> m_program2D, m_program3D;

    Camera *m_camera;

    PhysicsSystem m_physicsSystem;
    std::vector<std::shared_ptr<Particle>> m_particles;
    std::vector<std::shared_ptr<Spring>> m_springs; // To load springs from file
    std::vector<std::shared_ptr<TriangleCollider>> m_triangleColliders; // To load triangle colliders from file
    Model *m_model; // To load model from file
    
    // Custom Parametric Model
    BezierCurve m_curve;
    std::vector<QVector3D> m_curvePoints; // Control points of the curve
    bool m_isCurve { false };
    int m_numSamples { 32 };

    // Global settings
    float m_globalFriction { 0.95f };
    QVector3D m_globalRotation { 0.0f, 0.0f, 0.0f };

    // Model settings
    bool m_crossSpringModel { true }; // Cross spring model

    // Mode
    bool m_is2DMode { false };
    bool m_isPaused { true };
    bool m_isWireMode { false };

};