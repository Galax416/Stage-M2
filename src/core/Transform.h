#pragma once

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>

#include <cmath>

class Transform 
{
public:
    QVector3D position;
    QVector3D rotationEuler;
    QQuaternion rotation;
    QVector3D scale;

public:
    Transform() 
    {
        position      = QVector3D(0.0f, 0.0f, 0.0f);
        rotationEuler = QVector3D(0.0f, 0.0f, 0.0f);
        rotation      = QQuaternion::fromEulerAngles(rotationEuler);
        scale         = QVector3D(1.0f, 1.0f, 1.0f);
    }

    QMatrix4x4 GetModelMatrix() const 
    {
        QMatrix4x4 modelMatrix;
        modelMatrix.QMatrix4x4::translate(position);
        modelMatrix.QMatrix4x4::rotate(rotation);
        modelMatrix.QMatrix4x4::scale(scale);
        return modelMatrix;
    };

    QMatrix3x3 GetRotationMatrix() const 
    {
        return rotation.toRotationMatrix();
    };

    void SetRotation(const QQuaternion &rotation) 
    {
        this->rotation = rotation;
        this->rotationEuler = rotation.toEulerAngles();
    };

    void SetRotationEuler(const QVector3D &euler) 
    {
        this->rotationEuler = euler;
        this->rotationEuler.setX(fmod(this->rotationEuler.x(), 360.0f));
        this->rotationEuler.setY(fmod(this->rotationEuler.y(), 360.0f));
        this->rotationEuler.setZ(fmod(this->rotationEuler.z(), 360.0f));
        this->rotation = QQuaternion::fromEulerAngles(rotationEuler);
    };
};
