#pragma once

#include <QQuaternion>
#include <QPoint>
#include <QVector3D>
#include <QMatrix4x4>
#include <QtMath>

class Trackball
{
public:
    Trackball(float radius = 0.8f);

    void StartRotation(const QPoint& pos, int width, int height);
    QQuaternion UpdateRotation(const QPoint& pos, int width, int height, const QMatrix3x3& cameraBasis);
    QQuaternion GetRotation() const { return m_rotation; }
    void Reset();

private:
    QVector3D MapToSphere(const QPoint& point, int width, int height) const;

    float m_radius;
    QVector3D m_startVector;
    QQuaternion m_rotation;
};