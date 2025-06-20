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
    m_trackball = Trackball( 1.0f / m_rotationSpeed );
    ComputeView(m_viewMatrix, m_projectionMatrix);
}

void Camera::SetScreenSize(const int width, const int height) 
{
    m_width = width; 
    m_height = height; 
    m_aspect = static_cast<float>(width) / static_cast<float>(height);
    ComputeView(m_viewMatrix, m_projectionMatrix);
}

void Camera::ComputeView(QMatrix4x4& view, QMatrix4x4& projection)
{
    m_distance = (m_transform.position - m_target).length();

    projection.setToIdentity();
    if (m_isOrthographic) {
        float orthoSize = tan((m_fov / 2.0f) * M_PI / 180.0f) * m_distance;
        projection.ortho(-orthoSize * m_aspect, orthoSize * m_aspect, -orthoSize, orthoSize, m_near, m_far);
    }
    else {
        projection.perspective(m_fov, m_aspect, m_near, m_far);
    }

    QVector3D offset = m_orbitRotation.rotatedVector(QVector3D(0, 0, m_distance));
    QVector3D eye = m_target + offset;
    QVector3D up = m_orbitRotation.rotatedVector(WORLD_UP); // Up tourné avec l’orbite

    view.setToIdentity();
    view.lookAt(eye, m_target, up);

    // UpdateCameraRotation();
    UpdateCameraTransform();
}

void Camera::UpdateCameraTransform()
{
    // Update the camera transform based on the current position, target, and rotation
    m_transform.position = m_target + m_orbitRotation.rotatedVector(QVector3D(0, 0, m_distance));
    m_transform.rotation = m_orbitRotation;
    m_transform.rotationEuler = m_orbitRotation.toEulerAngles();
}

void Camera::SetPositionAndTarget(const QVector3D& position, const QVector3D& target)
{
    m_transform.position = position;
    m_target = target;

    m_distance = (m_transform.position - m_target).length();
    QVector3D front = (m_transform.position - m_target).normalized();

    QVector3D defaultForward = QVector3D(0, 0, -1); // Default forward vector
    QQuaternion rotationToFront;
    if (QVector3D::dotProduct(front, defaultForward) < -0.999f) rotationToFront = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180.0f);
    else rotationToFront = QQuaternion::rotationTo(defaultForward, front);
    
    // Ajuster la rotation orbitale (m_orbitRotation)
    m_orbitRotation = rotationToFront;

    // Mettre à jour la matrice de vue et projection
    ComputeView(m_viewMatrix, m_projectionMatrix);
}

void Camera::mousePressEvent(QMouseEvent* event)
{   
    m_lastMousePosition = event->pos();

    if (event->button() & Qt::LeftButton)  
    {
        m_isLeftMouseButtonPressed  = true;
        m_trackball.StartRotation(event->pos(), m_width, m_height);
    }
    if (event->button() & Qt::RightButton) m_isRightMouseButtonPressed = true;
}

void Camera::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() & Qt::LeftButton)  
    {
        m_isLeftMouseButtonPressed  = false;
        // m_trackball.Reset();
    }
    if (event->button() & Qt::RightButton) m_isRightMouseButtonPressed = false;
}

void Camera::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isLeftMouseButtonPressed)
    {
        // Left mouse button pressed, rotate the camera (Trackball)
        QQuaternion delta = m_trackball.UpdateRotation(event->pos(), m_width, m_height, GetCameraBasis());
        m_orbitRotation = delta * m_orbitRotation;
        m_orbitRotation.normalize();

    }
    else if (m_isRightMouseButtonPressed)
    {
        // Right mouse button pressed, move the camera (Pan)
        QPoint delta = event->pos() - m_lastMousePosition;

        QMatrix3x3 basis = GetCameraBasis();
        QVector3D right(basis(0,0), basis(1,0), basis(2,0));
        QVector3D up(basis(0,1), basis(1,1), basis(2,1));

        m_target += (-right * delta.x() + up * delta.y()) * m_moveSpeed;

    }

    // Update the view matrix
    ComputeView(m_viewMatrix, m_projectionMatrix);

    // Update the last mouse position
    m_lastMousePosition = event->pos();
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

QMatrix3x3 Camera::GetCameraBasis() const
{
    QMatrix3x3 basis;
    QVector3D right = m_orbitRotation.rotatedVector(QVector3D(1, 0, 0));   // right
    QVector3D up = m_orbitRotation.rotatedVector(WORLD_UP);                // up
    QVector3D front = m_orbitRotation.rotatedVector(QVector3D(0, 0, 1));   // front

    for (int i = 0; i < 3; ++i) {
        basis(i, 0) = right[i];
        basis(i, 1) = up[i];
        basis(i, 2) = front[i];
    }
    return basis;
}