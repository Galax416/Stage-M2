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

#include "Voxel.h"

class ModelSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ModelSettingsWidget(QWidget *parent = nullptr);
    // ~ModelSettingsWidget() override;

signals:
    void CrossSpringModelChanged(bool checked);
    void CreateBreastModelButtonClicked(bool clicked);
    void Create3DModelButtonClicked(bool clicked);
    void UpdateBreastModelButtonChanged(bool clicked);
    void Update3DModelButtonChanged(bool clicked);
    void DeformModelThicknessChanged(bool checked);
    void DeformModelSamplingChanged(int samplingModel);
    void DeformModelLayerChanged(int layerModel);
    void DeformModelCurveChanged(int p1, int p2, float value);
    void DeformModelRingChanged(float radius);
    void DeformModelHeightChanged(float height);
    void DeformModelWidthChanged(float width);
    void DeformModelDepthChanged(float depth);

public slots:
    void ResetSliders();
    void UpdateBreastModelButton(bool clicked);
    void Update3DModelButton(bool clicked);
    void Update3DModelParameters(VoxelGrid voxel);

private:
    void InitUI();
    void InitConnections();

    QVBoxLayout *m_mainLayout;

    QGroupBox   *m_modelSettingsGroupBox;
    QVBoxLayout *m_modelSettingsLayout;

    QCheckBox   *m_crossSpringModel;


    // Breast Model
    QPushButton *m_createBreastModelButton;
    bool m_createBreastModelButtonClicked { false };

    QGroupBox *m_deformBreastModelGroupBox;
    QCheckBox *m_thicknessCheckBox;
    QSpinBox  *m_samplingBreastModelSpinBox;
    QSpinBox  *m_layerBreastModelSpinBox;

    // Sliders for model deformation
    QSlider *m_sWidth, *m_sHeight, *m_sDepth;
    QSlider *m_s1, *m_s2, *m_s3, *m_s4; 
    QSlider *m_sRing;


    // 3D Model (voxel)
    QPushButton *m_create3DModelButton;
    bool m_create3DModelButtonClicked { false };

    QGroupBox   *m_parameters3DModelGroupBox;
    QVBoxLayout *m_parameters3DModelLayout;
};