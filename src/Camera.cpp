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
    m_front = (m_target - position).normalized();
    m_distance = (position - m_target).length();

    rotationEuler.setX(qRadiansToDegrees(asin(m_front.y())));
    rotationEuler.setY(qRadiansToDegrees(atan2(m_front.x(), -m_front.z())));
    rotationEuler.setZ(0.0f);
    rotation = QQuaternion::fromEulerAngles(rotationEuler);

    QVector3D right = QVector3D::crossProduct(m_front, WORLD_UP).normalized();
    QVector3D up = QVector3D::crossProduct(right, m_front).normalized();

    ComputeView(m_viewMatrix, m_projectionMatrix, up);
}

void Camera::ComputeView(QMatrix4x4& view, QMatrix4x4& projection, QVector3D up)
{
    if (m_is2DMode) {
        view.setToIdentity();
        projection.setToIdentity();
        return;
    }

    projection.setToIdentity();
    if (m_isOrthographic) {
        float orthoSize = tan((m_fov / 2.0f) * M_PI / 180.0f) * m_distance;
        projection.ortho(-orthoSize * m_aspect, orthoSize * m_aspect, -orthoSize, orthoSize, m_near, m_far);
    }
    else {
        projection.perspective(m_fov, m_aspect, m_near, m_far);
    }

    view.setToIdentity();
    view.lookAt(position, m_target, up);

    m_front = (m_target - position).normalized();
}

void Camera::UpdateCameraPosition()
{
    float yawRad = qDegreesToRadians(rotationEuler.y());
    float pitchRad = qDegreesToRadians(rotationEuler.x());

    float x = m_distance * cos(pitchRad) * sin(yawRad);
    float y = m_distance * sin(pitchRad);
    float z = m_distance * cos(pitchRad) * cos(yawRad);

    position = m_target + QVector3D(x, y, z);

    QVector3D right = QVector3D::crossProduct(m_front, WORLD_UP).normalized();
    QVector3D up = QVector3D::crossProduct(right, m_front).normalized();

    ComputeView(m_viewMatrix, m_projectionMatrix, up);
}

void Camera::mousePressEvent(QMouseEvent* event)
{   
    m_lastMousePosition = event->pos();
    if (event->button() & Qt::MiddleButton) m_isWheelPressed = true;

}

void Camera::mouseReleaseEvent(QMouseEvent* event)
{
    m_isWheelPressed = false;
}

void Camera::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_isWheelPressed) return;

    QPoint delta = event->pos() - m_lastMousePosition;
    m_lastMousePosition = event->pos();

    float dx = delta.x() * m_rotationSpeed;
    float dy = delta.y() * m_rotationSpeed;

    if ( m_is2DMode)
    {

    }
    else
    {
        rotationEuler.setX(rotationEuler.x() + dy);
        rotationEuler.setY(rotationEuler.y() - dx);
        
        rotationEuler.setX(qBound(-89.0f, rotationEuler.x(), 89.0f)); // Clamp pitch to avoid gimbal lock
        rotationEuler.setY(fmod(rotationEuler.y(), 360.0f)); // Wrap yaw to [0, 360]
        
        rotation = QQuaternion::fromEulerAngles(rotationEuler);

        UpdateCameraPosition(); 
        
    }
    

}

void Camera::wheelEvent(QWheelEvent* event)
{
    float delta = qBound(-120.0f, (float)event->angleDelta().y(), 120.0f) / 120.0f; // Normalize to -1, 0, or 1
    m_distance *= (1.0f - delta * m_zoomSpeed);  // Adjust distance with zoom sensitivity

    // Ensure distance remains within reasonable bounds
    float minDistance = 0.5f;
    float maxDistance = 100.0f;
    if (m_distance < minDistance) m_distance = minDistance;
    if (m_distance > maxDistance) m_distance = maxDistance;
    
    UpdateCameraPosition();
    // QVector3D direction = (position - m_target).normalized();
    // position = m_target + direction * m_distance;


    // ComputeView(m_viewMatrix, m_projectionMatrix);
}