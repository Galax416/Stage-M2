#pragma once

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QtMath>
#include <QWheelEvent>

#include "Transform.h"
#include "Trackball.h"

class Camera
{
public:
    Camera();
    Camera(const QVector3D& position, const QVector3D& target);

    // Setters
    void SetPosition(const QVector3D& position)   { m_transform.position = position; SetPositionAndTarget(m_transform.position, m_target); }
    // void SetRotation(const QQuaternion& rotation) { m_transform.SetRotation(rotation); ComputeView(m_viewMatrix, m_projectionMatrix); }
    // void SetRotation(const QVector3D& euler)      { m_transform.SetRotationEuler(euler); ComputeView(m_viewMatrix, m_projectionMatrix); }
    void SetTarget(const QVector3D& target)       { m_target = target; SetPositionAndTarget(m_transform.position, m_target); }
    // void SetFront(const QVector3D& front)         { m_front = front; ComputeView(m_viewMatrix, m_projectionMatrix); }
    void SetFov(const float fov)                  { m_fov = fov; ComputeView(m_viewMatrix, m_projectionMatrix); }
    void SetAspect(const float aspect)            { m_aspect = aspect; ComputeView(m_viewMatrix, m_projectionMatrix); }
    void SetNear(const float near)                { m_near = near; ComputeView(m_viewMatrix, m_projectionMatrix); }
    void SetFar(const float far)                  { m_far = far; ComputeView(m_viewMatrix, m_projectionMatrix); }
    void SetScreenSize(const int width, const int height);

    // Getters
    QVector3D GetPosition()          const { return m_transform.position; }
    QQuaternion GetRotation()        const { return m_transform.rotation; }
    QVector3D GetRotationEuler()     const { return m_transform.rotationEuler; }
    QVector3D GetTarget()            const { return m_target; }
    // QVector3D GetFront()             const { return m_front; }
    float GetDistance()              const { return m_distance; }
    float GetFov()                   const { return m_fov; }
    float GetAspect()                const { return m_aspect; }
    float GetNear()                  const { return m_near; }
    float GetFar()                   const { return m_far; }
    QMatrix4x4 GetViewMatrix()       const { return m_viewMatrix; }
    QMatrix4x4 GetProjectionMatrix() const { return m_projectionMatrix; }
    QVector3D GetDownVector()        const { return m_orbitRotation.rotatedVector(QVector3D(0, -1, 0)).normalized(); }

    // Events
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:
    void Initialize();
    void ComputeView(QMatrix4x4& view, QMatrix4x4& projection);
    void UpdateCameraTransform();
    void SetPositionAndTarget(const QVector3D& position, const QVector3D& target);
    QMatrix3x3 GetCameraBasis() const;

    // Transform
    Transform m_transform;

    // Trackball
    Trackball m_trackball;
    QQuaternion m_orbitRotation { QQuaternion() };

    // Parameters
    QVector3D m_target   { QVector3D(0.0f, 0.0f, 0.0f) };
    // QVector3D m_front    { QVector3D(0.0f, 0.0f, -1.0f) };
    float m_fov      { 60.0f };
    float m_aspect   { 4.0f / 3.0f };
    float m_near     { 0.001f };
    float m_far      { 1000.0f };
    float m_distance { 0.0f };
    int m_width, m_height;

    // View
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    // Mouse
    QPoint m_lastMousePosition;
    bool m_isLeftMouseButtonPressed  { false };
    bool m_isRightMouseButtonPressed { false };

    // Speed and sensibility
    float m_moveSpeed     { 0.005f };
    float m_rotationSpeed { 0.5f };
    float m_zoomSpeed     { 0.05f };
    
    // Camera mode
    // bool m_is2DMode         { false };
    bool m_isOrthographic   { false };
};