#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QVector>

#include "Constants.h"
#include "Camera.h"
#include "PhysicsSystem.h"

#include <memory>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

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

private:
    void initShaders(QOpenGLShaderProgram *program, QString vertex_shader, QString fragment_shader);
    QOpenGLShaderProgram *m_program;

    Camera *m_camera;
    // QOpenGLVertexArrayObject m_vao;

    PhysicsSystem m_physicsSystem;
    QVector<Particle> m_particles;
    QVector<Model> m_models;

    float deltaTime { 0.0005f };

    bool m_isPaused { true };

};