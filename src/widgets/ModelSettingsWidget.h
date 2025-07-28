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
#include <QSignalBlocker>
#include <QDebug>

#include "Voxel.h"

// Default Value of Sliders
#define DEFAULT_SAMPLING               44
#define DEFAULT_LAYERS                 5
#define DEFAULT_WIDTH                  47
#define DEFAULT_HEIGHT                 57
#define DEFAULT_SIZE                   45
#define DEFAULT_DEPTH                  50
#define DEFAULT_CURVE_FORM             50
#define DEFAULT_RING_RADIUS            50
#define DEFAULT_SPACING_VOLUME         50
#define DEFAULT_PARTICLE_RADIUS_VOLUME 50
#define DEFAULT_N_SEGMENTS             3

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
    void DeformModelAttachedChanged(bool checked);
    void DeformModelAttachedToModelChanged(bool checked);
    void AddParticleButtonClicked();
    void DeformModelSamplingChanged(int samplingModel);
    void DeformModelLayerChanged(int layerModel);
    void DeformModelCurveChanged(int p1, int p2, float value);
    void DeformModelRingChanged(float radius);
    void DeformModelHeightChanged(float height);
    void DeformModelWidthChanged(float width);
    void DeformModelSizeChanged(float size);
    void DeformModelDepthChanged(float depth);
    void DeformModelSpacingVolumeChanged(float spacing);
    void DeformModelParticleRadiusVolumeChanged(float radius);
    void UpdateNSegmentsChanged(int nSegments);
    void SegmentParameterChanged(std::vector<float>& values);

public slots:  
    void ClearSceneSlot();
    void ResetSliders();
    void UpdateBreastModelButton(bool clicked);
    void Update3DModelButton(bool clicked);
    void Update3DModelParameters(VoxelGrid voxel);
    void SetSizeSlider(int value);
    void SetDeformationSlider(int p1, int p2, int value);
    void SetSamplingModel(int value);
    void SetCurveSize(int value);
    void SetCurveDepth(int value);
    void SetRingRadius(int value);
    void SetParticleRadiusVolume(int value);
    void SetSpacingVolume(int value);
    void SetAttachedCheckBox(bool checked);
    void SetAttachedToModelCheckBox(bool checked);

private:
    void InitUI();
    void InitConnections();

    void UpdateNSegments(int nSegments);

    QVBoxLayout *m_mainLayout;

    QGroupBox   *m_modelSettingsGroupBox;
    QVBoxLayout *m_modelSettingsLayout;

    QCheckBox   *m_crossSpringModel;

    // Breast Model
    QPushButton *m_createBreastModelButton;
    bool m_createBreastModelButtonClicked { false };

    QGroupBox *m_deformBreastModelGroupBox;
    QCheckBox *m_thicknessCheckBox;
    QCheckBox *m_attachedCheckBox;
    QCheckBox *m_attachedToModelCheckBox;
    QPushButton *m_addParticleButton;
    QSpinBox  *m_samplingBreastModelSpinBox;
    QSpinBox  *m_layerBreastModelSpinBox;

    // Sliders for model deformation
    QSlider *m_sWidth, *m_sHeight, *m_sSize, *m_sDepth;
    QSlider *m_s1, *m_s2, *m_s3, *m_s4; 
    QPushButton *m_resetSlidersButton;
    QSlider *m_sRing;
    QSlider *m_sSpacingVolume, *m_sParticleRadiusVolume;
    
    // Parameters for model deformation
    QGroupBox *m_segmentGroupBox;
    QVBoxLayout *m_segmentLayout;
    QSpinBox *m_nSegmentSpinBox;
    QVBoxLayout *m_segmentParametersLayout;
    std::vector<QSlider*> m_segmentSliders; // Segment sliders (angle values)
    std::vector<int> m_previousValues; // Store previous values of segment sliders
    bool m_updating = false;

    // 3D Model (voxel)
    QPushButton *m_create3DModelButton;
    bool m_create3DModelButtonClicked { false };

    QGroupBox   *m_parameters3DModelGroupBox;
    QVBoxLayout *m_parameters3DModelLayout;
};