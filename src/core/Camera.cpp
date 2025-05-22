#include "Camera.h"

const QVector3D WORLD_UP(0.0f, 1.0f, 0.0f);

Camera::Camera()
{
    Initialize();
}

Camera::Camera(const QVector3D& position, const QVector3D& target)
{
    m_transform.position = position;
    m_target = target;

    Initialize();
}

void Camera::Initialize()
{
    ComputeView(m_viewMatrix, m_projectionMatrix);
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
    view.lookAt(m_transform.position, m_target, up);

    m_front = (m_target - m_transform.position).normalized();
    m_distance = (m_transform.position - m_target).length();

    UpdateCameraRotation();
}

void Camera::UpdateCameraRotation()
{
    QVector3D right = QVector3D::crossProduct(m_front, WORLD_UP).normalized();
    QVector3D up    = QVector3D::crossProduct(right, m_front).normalized();

    QMatrix4x4 rotationMatrix;
    rotationMatrix.setColumn(0, QVector4D(right, 0.0f));
    rotationMatrix.setColumn(1, QVector4D(up, 0.0f));
    rotationMatrix.setColumn(2, QVector4D(-m_front, 0.0f));
    rotationMatrix.setColumn(3, QVector4D(0, 0, 0, 1));

    m_transform.rotation = QQuaternion::fromRotationMatrix(rotationMatrix.toGenericMatrix<3,3>());
    m_transform.rotationEuler = m_transform.rotation.toEulerAngles();
}

void Camera::mousePressEvent(QMouseEvent* event)
{   
    m_lastMousePosition = event->pos();
    if (event->button() & Qt::LeftButton) m_isLeftMouseButtonPressed = true;
    if (event->button() & Qt::RightButton) m_isRightMouseButtonPressed = true;

}

void Camera::mouseReleaseEvent(QMouseEvent* event)
{
    m_isLeftMouseButtonPressed = false;
    m_isRightMouseButtonPressed = false;
}

void Camera::mouseMoveEvent(QMouseEvent* event)
{
    QPoint delta = event->pos() - m_lastMousePosition;
    m_lastMousePosition = event->pos();

    float dx = delta.x() * m_rotationSpeed;
    float dy = delta.y() * m_rotationSpeed;

    if (m_isLeftMouseButtonPressed)
    {
        if ( m_is2DMode)
        {
            // TODO: Implement 2D camera rotation
        }
        else
        {
            QVector3D direction = m_transform.position - m_target;
            float radius = direction.length();
    
            float theta = qAtan2(direction.z(), direction.x()); 
            float phi = qAcos(direction.y() / radius);           
    
            theta += qDegreesToRadians(dx);
            phi   -= qDegreesToRadians(dy);
    
            const float epsilon = 0.01f;
            phi = qBound(epsilon, phi, float(M_PI) - epsilon);
    
            float x = radius * qSin(phi) * qCos(theta);
            float y = radius * qCos(phi);
            float z = radius * qSin(phi) * qSin(theta);
    
            m_transform.position = m_target + QVector3D(x, y, z);
    
            m_front = (m_target - m_transform.position).normalized();
            
        }
    }
    else if (m_isRightMouseButtonPressed)
    {
        if (m_is2DMode)
        {
            // Nothing to do
        }
        else 
        {
            // Right mouse button pressed, rotate the camera
            QVector3D right = QVector3D::crossProduct(m_front, WORLD_UP).normalized();
            QVector3D up    = QVector3D::crossProduct(right, m_front).normalized();

            // Apply rotation
            m_transform.position += -dx * right * m_moveSpeed;
            m_transform.position += dy * up * m_moveSpeed;

            // Update the target position
            m_target += -dx * right * m_moveSpeed;
            m_target += dy * up * m_moveSpeed;
        }
    }

    ComputeView(m_viewMatrix, m_projectionMatrix);
    
}

void Camera::wheelEvent(QWheelEvent* event)
{
    float delta = qBound(-120.0f, (float)event->angleDelta().y(), 120.0f) / 120.0f; // Normalize to -1, 0, or 1
    m_distance *= (1.0f - delta * m_zoomSpeed);  // Adjust distance with zoom sensitivity

    QVector3D direction = (m_transform.position - m_target).normalized();
    m_transform.position = m_target + direction * m_distance;

    // Ensure distance remains within reasonable bounds
    float minDistance = 0.5f;
    float maxDistance = 100.0f;
    if (m_distance < minDistance) m_distance = minDistance;
    if (m_distance > maxDistance) m_distance = maxDistance;
    
    ComputeView(m_viewMatrix, m_projectionMatrix);

}