#include "Trackball.h"

Trackball::Trackball(float radius) : m_radius(radius), m_rotation() {}

void Trackball::StartRotation(const QPoint &point, int width, int height)
{
    m_startVector = MapToSphere(point, width, height);
}

QQuaternion Trackball::UpdateRotation(const QPoint& pos, int width, int height, const QMatrix3x3& basis)
{
    QVector3D currentVector = MapToSphere(pos, width, height);
    QVector3D axis = QVector3D::crossProduct(currentVector, m_startVector);
    float angle = std::acos(std::min(1.0f, QVector3D::dotProduct(currentVector, m_startVector)));

    QQuaternion delta;
    if (!axis.isNull())
    {
        // Manually multiply QMatrix3x3 by QVector3D
        QVector3D transformedAxis(
            QVector3D::dotProduct(QVector3D(basis(0,0), basis(0,1), basis(0,2)), axis),
            QVector3D::dotProduct(QVector3D(basis(1,0), basis(1,1), basis(1,2)), axis),
            QVector3D::dotProduct(QVector3D(basis(2,0), basis(2,1), basis(2,2)), axis)
        );
        axis = transformedAxis;
        QQuaternion delta = QQuaternion::fromAxisAndAngle(axis.normalized(), qRadiansToDegrees(angle));
        m_rotation = delta;
    }

    m_startVector = currentVector;
    return m_rotation;
}

void Trackball::Reset()
{
    m_rotation = QQuaternion();
}

QVector3D Trackball::MapToSphere(const QPoint &point, int width, int height) const
{
    float x = (2.0f * point.x() - width) / width;
    float y = (height - 2.0f * point.y()) / height;

    x *= m_radius; y *= m_radius;

    float lengthSquared = x * x + y * y;
    float radiusSquared = m_radius * m_radius;

    float z = lengthSquared < radiusSquared ? std::sqrt(radiusSquared - lengthSquared) : 0.0f;

    return QVector3D(x, y, z).normalized();
}
