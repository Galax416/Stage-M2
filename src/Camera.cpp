#include "Camera.h"

Camera::Camera()
{
    Initialize();
}

Camera::Camera(const QVector3D& position, const QVector3D& target)
{
    this->position = position;
    m_target = target;
    Initialize();
}

void Camera::Initialize()
{
    QVector3D direction = (m_target - position).normalized();
    rotationEuler.setX(qRadiansToDegrees(atan2(direction.y(), direction.z())));
    rotationEuler.setY(qRadiansToDegrees(atan2(direction.x(), direction.z())));
    rotationEuler.setZ(0.0f);
    rotation = QQuaternion::fromEulerAngles(rotationEuler);
    ComputeView(m_viewMatrix, m_projectionMatrix);
}

void Camera::ComputeView(QMatrix4x4& view, QMatrix4x4& projection)
{
    projection.setToIdentity();
    if (m_isOrthographic) {
        float distanceToTarget = (position - m_target).length();
        float orthoSize = tan((m_fov / 2.0f) * M_PI / 180.0f) * distanceToTarget;
        projection.ortho(-orthoSize * m_aspect, orthoSize * m_aspect, -orthoSize, orthoSize, m_near, m_far);
    }
    else {
        projection.perspective(m_fov, m_aspect, m_near, m_far);
    }
    m_projectionMatrix = projection;

    view.setToIdentity();
    view.lookAt(position, m_target, WORLD_UP);
    m_viewMatrix = view;

    m_front = (m_target - position).normalized();
}

void Camera::mousePressEvent(QMouseEvent* event)
{   
    m_lastMousePosition = event->pos();
    if (event->button() & Qt::MiddleButton) m_isWheelPressed = true;

}

void Camera::mouseReleaseEvent(QMouseEvent* event)
{
    /* if (event->button() == Qt::MiddleButton) */ m_isWheelPressed = false;
}

void Camera::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_isWheelPressed) return;

    QPoint delta = event->pos() - m_lastMousePosition;
    float dx = delta.x() * m_rotationSpeed;
    float dy = delta.y() * m_rotationSpeed;

    QQuaternion rotationX = QQuaternion::fromAxisAndAngle(WORLD_UP, -dx);
    QVector3D right = QVector3D::crossProduct(WORLD_UP, m_target - position).normalized();
    QQuaternion rotationY = QQuaternion::fromAxisAndAngle(right, dy);

    rotation = rotationX * rotationY;
    rotationEuler = rotation.toEulerAngles();
    position = m_target + rotation.rotatedVector(position - m_target);
    
    m_lastMousePosition = event->pos();
    
    ComputeView(m_viewMatrix, m_projectionMatrix);

}

void Camera::wheelEvent(QWheelEvent* event)
{
    float delta = qBound(-120.0f, (float)event->angleDelta().y(), 120.0f) / 120.0f; // Normalize to -1, 0, or 1
    float distance = (position - m_target).length();
    distance *= (1.0f - delta * m_zoomSpeed);  // Adjust distance with zoom sensitivity

    QVector3D direction = (position - m_target).normalized();
    position = m_target + direction * distance;

    // Ensure distance remains within reasonable bounds
    float minDistance = 0.5f;
    float maxDistance = 100.0f;
    if (distance < minDistance) distance = minDistance;
    if (distance > maxDistance) distance = maxDistance;

    ComputeView(m_viewMatrix, m_projectionMatrix);
}