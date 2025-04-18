#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QVector3D>
#include <QMap>
#include <QDebug>

class ModelSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ModelSettingsWidget(QWidget *parent = nullptr);
    ~ModelSettingsWidget() override;

signals:
    void CrossSpringModelChanged(bool checked);
    void CreateModelButtonClicked(bool clicked);
    void DeformModelSamplingChanged(int samplingModel);
    void DeformModelLayerChanged(int layerModel);
    void DeformModelCurveChanged(int p1, int p2, float value);
    void DeformModelRingChanged(float ringW, float ringH);
    void DeformModelHeightChanged(float height);

private:
    void InitUI();
    void InitConnections();

    QVBoxLayout *m_mainLayout;

    QGroupBox   *m_modelSettingsGroupBox;
    QVBoxLayout *m_modelSettingsLayout;

    QCheckBox   *m_crossSpringModel;
    QPushButton *m_createModelButton;
    bool m_createModelButtonClicked { false };

    QGroupBox *m_deformModelGroupBox;
    QSpinBox  *m_samplingModelSpinBox;
    QSpinBox  *m_layerModelSpinBox;

    // Sliders for model deformation
    QSlider *m_s1, *m_s2, *m_s3, *m_s4, *m_s5; 
    QSlider *m_sH, *m_sRingW, *m_sRingH;
};