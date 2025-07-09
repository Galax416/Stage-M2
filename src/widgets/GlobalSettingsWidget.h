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
    void ClearSceneButtonClicked();
    void DeltaTimeChanged(float value);
    void FrictionChanged(float value);
    void RotationChanged(QVector3D rotation);
    void BackgroundColorChanged(QColor color);
    void GravityChanged();

private:
    void InitUI();
    void InitConnections();

    QHBoxLayout *m_mainLayout;

    QGroupBox   *m_globalSettingsGroupBox;

    QPushButton *m_clearSceneButton;
    // QPushButton *m_changeGravityButton;

    QGroupBox   *m_dtGroupBox;
    QLineEdit   *m_dtLineEdit;
    QPushButton *m_dtButton;

    QGroupBox *m_frictionGroupBox;
    QSlider   *m_frictionSlider;
    QSpinBox  *m_frictionSpinBox;

    QGroupBox   *m_backgroundColorGroupBox;
    QPushButton *m_backgroundColorButton;

    QGroupBox   *m_gravityGroupBox;
    QPushButton *m_gravityButton;

    /*
    QGroupBox *m_rotationGroupBox;
    QSlider   *m_rotationXSlider;
    QSpinBox  *m_rotationXSpinBox;
    QSlider   *m_rotationYSlider;
    QSpinBox  *m_rotationYSpinBox;
    QSlider   *m_rotationZSlider;
    QSpinBox  *m_rotationZSpinBox;
    */


};