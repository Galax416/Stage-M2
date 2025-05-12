#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QColorDialog>
#include <QVector3D>
#include <QDebug>

class GlobalSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GlobalSettingsWidget(QWidget *parent = nullptr);
    // ~GlobalSettingsWidget() override;

signals:
    void FrictionChanged(float value);
    void DeltaTimeChanged(float value);
    void RotationChanged(QVector3D rotation);
    void BackgroundColorChanged(QColor color);

private:
    void InitUI();
    void InitConnections();

    QHBoxLayout *m_mainLayout;

    QGroupBox   *m_globalSettingsGroupBox;

    QGroupBox *m_dtGroupBox;
    QLineEdit *m_dtLineEdit;

    QGroupBox *m_frictionGroupBox;
    QSlider   *m_frictionSlider;
    QSpinBox  *m_frictionSpinBox;

    QGroupBox   *m_backgroundColorGroupBox;
    QPushButton *m_backgroundColorButton;

    QGroupBox *m_rotationGroupBox;
    QSlider   *m_rotationXSlider;
    QSpinBox  *m_rotationXSpinBox;
    QSlider   *m_rotationYSlider;
    QSpinBox  *m_rotationYSpinBox;
    QSlider   *m_rotationZSlider;
    QSpinBox  *m_rotationZSpinBox;


};