#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QVector3D>
#include <QDebug>

#include "Constants.h"

class GlobalSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalSettingsWidget(QWidget *parent = nullptr);
    ~GlobalSettingsWidget() override;

signals:
    void FrictionChanged(float value);
    void DeltaTimeChanged(float value);
    void RotationChanged(QVector3D rotation);

private:
    void InitUI();
    void InitConnections();

    QVBoxLayout* m_mainLayout;

    QGroupBox* m_globalSettingsGroupBox;

    QGroupBox* m_dtGroupBox;
    QLineEdit* m_dtLineEdit;

    QGroupBox* m_frictionGroupBox;
    QSlider* m_frictionSlider;
    QSpinBox* m_frictionSpinBox;

    QGroupBox* m_rotationGroupBox;
    QSlider* m_rotationXSlider;
    QSpinBox* m_rotationXSpinBox;
    QSlider* m_rotationYSlider;
    QSpinBox* m_rotationYSpinBox;
    QSlider* m_rotationZSlider;
    QSpinBox* m_rotationZSpinBox;


};