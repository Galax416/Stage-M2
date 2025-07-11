#include "ModelSettingsWidget.h"
#include "Spring.h"
#include "Utils.h"

#include <memory>


ModelSettingsWidget::ModelSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(300);
    setMaximumWidth(400);
    InitUI();
    InitConnections();
}

// ModelSettingsWidget::~ModelSettingsWidget()
// {

// }

void ModelSettingsWidget::InitUI()
{
    qDebug() << "Initializing ModelSettingsWidget UI";
    // Set the layout for the widget
    m_mainLayout = new QVBoxLayout(this);
    
    // Model settings group box
    m_modelSettingsGroupBox = new QGroupBox("Model Settings", this);
    m_modelSettingsGroupBox->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");
    m_modelSettingsGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    // m_modelSettingsGroupBox->setCheckable(true);
    // m_modelSettingsGroupBox->setChecked(false);

    m_modelSettingsLayout = new QVBoxLayout(m_modelSettingsGroupBox);
    
    { // Cross Button
        // m_crossSpringModel = new QCheckBox("Cross Spring", this);
        // m_crossSpringModel->setStyleSheet("QCheckBox { font-size: 12px; }");
        // m_crossSpringModel->setChecked(true);
        // // m_crossSpringModel->setVisible(false);
        // m_modelSettingsLayout->addWidget(m_crossSpringModel);
    }

    m_createBreastModelButton = new QPushButton("Create Breast Model", this);
    m_createBreastModelButton->setStyleSheet("QPushButton { font-size: 12px; }");
    // m_createModelButton->setVisible(false);
    m_modelSettingsLayout->addWidget(m_createBreastModelButton);

    {
        m_deformBreastModelGroupBox = new QGroupBox("Deform Model", this);
        m_deformBreastModelGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
        m_deformBreastModelGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_deformBreastModelGroupBox->setVisible(false); // Initially hidden

        auto deformModelLayout = new QVBoxLayout(m_deformBreastModelGroupBox);

        { // Thickness
            // m_thicknessCheckBox = new QCheckBox("Thickness", this);
            // m_thicknessCheckBox->setStyleSheet("QCheckBox { font-size: 12px; }");
            // m_thicknessCheckBox->setChecked(true);
            // deformModelLayout->addWidget(m_thicknessCheckBox);
        }

        { // Attached
            auto atachedLayout = new QHBoxLayout();
            m_attachedCheckBox = new QCheckBox("Attached", this);
            m_attachedCheckBox->setStyleSheet("QCheckBox { font-size: 12px; }");
            m_attachedCheckBox->setChecked(false);
            atachedLayout->addWidget(m_attachedCheckBox);

            m_attachedToModelCheckBox = new QCheckBox("Attached to the model", this);
            m_attachedToModelCheckBox->setStyleSheet("QCheckBox { font-size: 12px; }");
            m_attachedToModelCheckBox->setChecked(false);
            atachedLayout->addWidget(m_attachedToModelCheckBox);

            // Add space between checkboxes and button
            atachedLayout->addStretch();

            m_addParticleButton = new QPushButton("Add Particle", this);
            m_addParticleButton->setStyleSheet("QPushButton { font-size: 12px; }");
            atachedLayout->addWidget(m_addParticleButton);

            deformModelLayout->addLayout(atachedLayout);
        }
        
        { // Sampling
            auto samplingLayout = new QHBoxLayout();
            auto samplingLabel = new QLabel("Resolution:", this);
            samplingLabel->setStyleSheet("QLabel { font-size: 12px; }");
            samplingLayout->addWidget(samplingLabel);
            m_samplingBreastModelSpinBox = new QSpinBox(this);
            m_samplingBreastModelSpinBox->setStyleSheet("QSpinBox { font-size: 12px; }");
            m_samplingBreastModelSpinBox->setRange(20, 100);
            m_samplingBreastModelSpinBox->setValue(DEFAULT_SAMPLING); // Default value
            m_samplingBreastModelSpinBox->setSingleStep(4);
            // m_samplingBreastModelSpinBox->setSuffix(" samples");
            samplingLayout->addWidget(m_samplingBreastModelSpinBox);
            deformModelLayout->addLayout(samplingLayout);
        }

        { // Layer
            // auto layerLayout = new QHBoxLayout();
            // auto layerLabel = new QLabel("Layer:", this);
            // layerLabel->setStyleSheet("QLabel { font-size: 12px; }");
            // layerLayout->addWidget(layerLabel);
            // m_layerBreastModelSpinBox = new QSpinBox(this);
            // m_layerBreastModelSpinBox->setStyleSheet("QSpinBox { font-size: 12px; }");
            // m_layerBreastModelSpinBox->setRange(1, 100);
            // m_layerBreastModelSpinBox->setValue(DEFAULT_LAYERS); // Default value
            // m_layerBreastModelSpinBox->setSingleStep(1);
            // m_layerBreastModelSpinBox->setSuffix(" layers");
            // layerLayout->addWidget(m_layerBreastModelSpinBox);
            // deformModelLayout->addLayout(layerLayout);
        }

        { // Width
            // QLabel *heightLabel = new QLabel("Width:", this);
            // heightLabel->setStyleSheet("QLabel { font-size: 12px; }");
            // deformModelLayout->addWidget(heightLabel);
            
            // m_sWidth = new QSlider(Qt::Horizontal, this);
            // m_sWidth->setRange(0, 100);
            // m_sWidth->setValue(DEFAULT_WIDTH); // Default value
            // m_sWidth->setSingleStep(1);
            // deformModelLayout->addWidget(m_sWidth);
        }

        { // Height
            // QLabel *heightLabel = new QLabel("Height:", this);
            // heightLabel->setStyleSheet("QLabel { font-size: 12px; }");
            // deformModelLayout->addWidget(heightLabel);
    
            // m_sHeight = new QSlider(Qt::Horizontal, this);
            // m_sHeight->setRange(0, 100);
            // m_sHeight->setValue(DEFAULT_HEIGHT); // Default value
            // m_sHeight->setSingleStep(1);
            // deformModelLayout->addWidget(m_sHeight);
        }

        { // Size
            QLabel *sizeLabel = new QLabel("Size:", this);
            sizeLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(sizeLabel);
    
            m_sSize = new QSlider(Qt::Horizontal, this);
            m_sSize->setRange(0, 100);
            m_sSize->setValue(DEFAULT_SIZE); // Default value
            m_sSize->setSingleStep(1);
            deformModelLayout->addWidget(m_sSize);
        }
        
        { // Depth
            QLabel *heightLabel = new QLabel("Depth:", this);
            heightLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(heightLabel);
    
            m_sDepth = new QSlider(Qt::Horizontal, this);
            m_sDepth->setRange(0, 100);
            m_sDepth->setValue(DEFAULT_DEPTH); // Default value
            m_sDepth->setSingleStep(1);
            deformModelLayout->addWidget(m_sDepth);
        }

        { // Slider form
            auto deformLabel = new QLabel("Form:", this);
            deformLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(deformLabel);
    
            m_s1 = new QSlider(Qt::Horizontal, this);
            m_s1->setRange(0, 100);
            m_s1->setValue(DEFAULT_CURVE_FORM); // Default value
            m_s1->setSingleStep(1);
            deformModelLayout->addWidget(m_s1);
    
            m_s2 = new QSlider(Qt::Horizontal, this);
            m_s2->setRange(0, 100);
            m_s2->setValue(DEFAULT_CURVE_FORM); // Default value
            m_s2->setSingleStep(1);
            deformModelLayout->addWidget(m_s2);
    
            m_s3 = new QSlider(Qt::Horizontal, this);
            m_s3->setRange(0, 100);
            m_s3->setValue(DEFAULT_CURVE_FORM); // Default value
            m_s3->setSingleStep(1);
            deformModelLayout->addWidget(m_s3);
    
            m_s4 = new QSlider(Qt::Horizontal, this);
            m_s4->setRange(0, 100);
            m_s4->setValue(DEFAULT_CURVE_FORM); // Default value
            m_s4->setSingleStep(1);
            deformModelLayout->addWidget(m_s4);

            m_resetSlidersButton = new QPushButton("Reset Sliders", this);
            m_resetSlidersButton->setStyleSheet("QPushButton { font-size: 12px; }");
            deformModelLayout->addWidget(m_resetSlidersButton);
    
        }

        { // Ring
            QLabel *ringLabel = new QLabel("Areola:", this);
            ringLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(ringLabel);
    
            m_sRing = new QSlider(Qt::Horizontal, this);
            m_sRing->setRange(0, 100);
            m_sRing->setValue(DEFAULT_RING_RADIUS); // Default value
            m_sRing->setSingleStep(1);
            deformModelLayout->addWidget(m_sRing);
        }

        { // Spacing Volume
            auto spacingLabel = new QLabel("Spacing:", this);
            spacingLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(spacingLabel);

            m_sSpacingVolume = new QSlider(Qt::Horizontal, this);
            m_sSpacingVolume->setStyleSheet("QSlider { font-size: 12px; }");
            m_sSpacingVolume->setRange(0, 100);
            m_sSpacingVolume->setValue(DEFAULT_SPACING_VOLUME); // Default value
            m_sSpacingVolume->setSingleStep(1);
            deformModelLayout->addWidget(m_sSpacingVolume);
        }

        { // Particle Radius Volume
            auto particleRadiusLabel = new QLabel("Particle Radius:", this);
            particleRadiusLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(particleRadiusLabel);

            m_sParticleRadiusVolume = new QSlider(Qt::Horizontal, this);
            m_sParticleRadiusVolume->setStyleSheet("QSlider { font-size: 12px; }");
            m_sParticleRadiusVolume->setRange(0, 100);
            m_sParticleRadiusVolume->setValue(DEFAULT_PARTICLE_RADIUS_VOLUME); // Default value
            m_sParticleRadiusVolume->setSingleStep(1);
            deformModelLayout->addWidget(m_sParticleRadiusVolume);
        }

        { // Segment
            m_segmentGroupBox = new QGroupBox("Segment", this);
            m_segmentGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
            m_segmentGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            m_segmentLayout = new QVBoxLayout(m_segmentGroupBox);
            auto segmentLayout = new QHBoxLayout();
            segmentLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            auto nSegmentLabel = new QLabel("Number of Segments:", this);
            nSegmentLabel->setStyleSheet("QLabel { font-size: 12px; }");
            segmentLayout->addWidget(nSegmentLabel);
            m_nSegmentSpinBox = new QSpinBox(this);
            m_nSegmentSpinBox->setStyleSheet("QSpinBox { font-size: 12px; }");
            m_nSegmentSpinBox->setRange(1, 4);
            m_nSegmentSpinBox->setValue(DEFAULT_N_SEGMENTS); // Default value
            m_nSegmentSpinBox->setSingleStep(1);
            m_nSegmentSpinBox->setSuffix(" segments");
            segmentLayout->addWidget(m_nSegmentSpinBox);
            m_segmentLayout->addLayout(segmentLayout);

            m_segmentParametersLayout = new QVBoxLayout();
            m_segmentParametersLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            m_segmentLayout->addLayout(m_segmentParametersLayout);

            UpdateNSegments(DEFAULT_N_SEGMENTS); // Initialize with default value

            deformModelLayout->addWidget(m_segmentGroupBox);
        }
        
        m_modelSettingsLayout->addWidget(m_deformBreastModelGroupBox);  
    }  

    m_create3DModelButton = new QPushButton("Create 3D Model", this);
    m_create3DModelButton->setStyleSheet("QPushButton { font-size: 12px; }");
    // m_create3DModelButton->setVisible(false);
    m_modelSettingsLayout->addWidget(m_create3DModelButton);

    {
        m_parameters3DModelGroupBox = new QGroupBox("Parameters", this);
        m_parameters3DModelGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
        m_parameters3DModelGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_parameters3DModelGroupBox->setVisible(false); // Initially hidden

        m_parameters3DModelLayout = new QVBoxLayout(m_parameters3DModelGroupBox);
        // deformModelLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        m_modelSettingsLayout->addWidget(m_parameters3DModelGroupBox);
    }

    m_modelSettingsGroupBox->setLayout(m_modelSettingsLayout);

    m_mainLayout->addWidget(m_modelSettingsGroupBox);
    
}

void ModelSettingsWidget::InitConnections()
{
    // connect(m_modelSettingsGroupBox, &QGroupBox::toggled, this, [=](bool checked) {
    //     int n = m_createModelButtonClicked ? m_modelSettingsLayout->count() : 2;
    //     for (int i = 0; i < n; ++i) {
    //         QWidget* widget = m_modelSettingsLayout->itemAt(i)->widget();
    //         if (widget) {
    //             widget->setVisible(checked);
    //         }
    //     }
    // });

    // connect(m_crossSpringModel, &QCheckBox::stateChanged, this, [=](int state) {
    //     emit CrossSpringModelChanged(state == Qt::Checked);
    // });

    connect(this, &ModelSettingsWidget::UpdateBreastModelButtonChanged, this, &ModelSettingsWidget::UpdateBreastModelButton);
    connect(this, &ModelSettingsWidget::Update3DModelButtonChanged, this, &ModelSettingsWidget::Update3DModelButton);

    connect(m_createBreastModelButton, &QPushButton::clicked, this, [=]() {
        m_createBreastModelButtonClicked = !m_createBreastModelButtonClicked;

        // m_thicknessCheckBox->setChecked(true);
        ResetSliders();

        emit Update3DModelButtonChanged(false); // Hide 3D model parameters
        emit UpdateBreastModelButtonChanged(m_createBreastModelButtonClicked);

    });

    // connect(m_thicknessCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
    //     emit DeformModelThicknessChanged(state == Qt::Checked);
    // });

    connect(m_attachedCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
        emit DeformModelAttachedChanged(state == Qt::Checked);
    });
    connect(m_attachedToModelCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
        emit DeformModelAttachedToModelChanged(state == Qt::Checked);
    });

    connect(m_addParticleButton, &QPushButton::clicked, this, [=]() {
        emit AddParticleButtonClicked();
    });

    connect(m_samplingBreastModelSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        emit DeformModelSamplingChanged(value);
    });

    // connect(m_layerBreastModelSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
    //     emit DeformModelLayerChanged(value);
    // });

    // connect(m_sWidth, &QSlider::valueChanged, this, [=](int value) {
    //     emit DeformModelWidthChanged((value * 0.02f) - 1.0f);
    // });

    // connect(m_sHeight, &QSlider::valueChanged, this, [=](int value) {
    //     emit DeformModelHeightChanged((value * 0.02f) - 1.0f);
    // });

    connect(m_sSize, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelSizeChanged((value * 0.02f) - 1.0f);
    });

    connect(m_sDepth, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelDepthChanged((value * 0.02f) - 1.0f);
    });

    connect(m_s1, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelCurveChanged(1, 9, (value * 0.02f) - 1.0f);
    });

    connect(m_s2, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelCurveChanged(2, 8, (value * 0.02f) - 1.0f);
    });

    connect(m_s3, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelCurveChanged(3, 7, (value * 0.02f) - 1.0f);
    });

    connect(m_s4, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelCurveChanged(4, 6, (value * 0.02f) - 1.0f);
    });

    connect(m_resetSlidersButton, &QPushButton::clicked, this, [=]() {
        m_s1->setValue(DEFAULT_CURVE_FORM); 
        m_s2->setValue(DEFAULT_CURVE_FORM); 
        m_s3->setValue(DEFAULT_CURVE_FORM); 
        m_s4->setValue(DEFAULT_CURVE_FORM); 
        emit DeformModelCurveChanged(1, 9, 0.0f);
        emit DeformModelCurveChanged(2, 8, 0.0f);
        emit DeformModelCurveChanged(3, 7, 0.0f);
        emit DeformModelCurveChanged(4, 6, 0.0f);
    });

    connect(m_sRing, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelRingChanged((value * 0.02f) - 1.0f);
    });

    connect(m_sSpacingVolume, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelSpacingVolumeChanged((value * 0.02f) - 1.0f);
    });

    connect(m_sParticleRadiusVolume, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelParticleRadiusVolumeChanged((value * 0.02f) - 1.0f);
    });

    connect(m_nSegmentSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        UpdateNSegments(value);
        emit UpdateNSegmentsChanged(value);
    });

    connect(m_create3DModelButton, &QPushButton::clicked, this, [=]() {
        m_create3DModelButtonClicked = !m_create3DModelButtonClicked;

        emit UpdateBreastModelButtonChanged(false);
        emit Update3DModelButtonChanged(m_create3DModelButtonClicked);
        
    });

}

void ModelSettingsWidget::ClearSceneSlot()
{
    // Clear the scene when the clear button is clicked
    m_createBreastModelButtonClicked = false;
    m_create3DModelButtonClicked = false;

    ResetSliders();

    UpdateBreastModelButton(false);
    Update3DModelButton(false);
}

void ModelSettingsWidget::ResetSliders()
{
    // Reset the sliders to their default values

    // m_sWidth->setValue(47); 
    // m_sHeight->setValue(57); 
    m_samplingBreastModelSpinBox->setValue(DEFAULT_SAMPLING); 
    m_sSize->setValue(DEFAULT_SIZE); 
    m_sDepth->setValue(DEFAULT_DEPTH); 
    m_s1->setValue(DEFAULT_CURVE_FORM); 
    m_s2->setValue(DEFAULT_CURVE_FORM); 
    m_s3->setValue(DEFAULT_CURVE_FORM); 
    m_s4->setValue(DEFAULT_CURVE_FORM); 
    m_sRing->setValue(DEFAULT_RING_RADIUS); 
    m_sSpacingVolume->setValue(DEFAULT_SPACING_VOLUME); 
    m_sParticleRadiusVolume->setValue(DEFAULT_PARTICLE_RADIUS_VOLUME); 

    for (auto& slider : m_segmentSliders) slider->setValue(360 / m_segmentSliders.size());

}


void ModelSettingsWidget::UpdateBreastModelButton(bool clicked)
{
    qDebug() << "UpdateBreastModelButton called with clicked:" << clicked;
    m_createBreastModelButtonClicked = clicked;
    m_createBreastModelButton->setText(clicked ? "Destroy Breast Model" : "Create Breast Model");
    m_deformBreastModelGroupBox->setVisible(clicked);
    emit CreateBreastModelButtonClicked(clicked);
}

void ModelSettingsWidget::Update3DModelButton(bool clicked)
{
    m_create3DModelButtonClicked = clicked;
    m_create3DModelButton->setText(clicked ? "Destroy 3D Model" : "Create 3D Model");
    m_parameters3DModelGroupBox->setVisible(clicked);
    emit Create3DModelButtonClicked(clicked);
}

void ModelSettingsWidget::UpdateNSegments(int nSegments)
{
    clearLayout(m_segmentParametersLayout);
    m_segmentSliders.clear();
    m_previousValues.clear();

    if (nSegments <= 1) return;

    int baseValue = 360 / nSegments;

    for (int i = 0; i < nSegments; ++i) {
        int value = baseValue; 

        auto slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(0, 360);
        slider->setValue(value);
        slider->setSingleStep(1);
        m_segmentSliders.push_back(slider);
        m_previousValues.push_back(value);

        connect(slider, &QSlider::valueChanged, this, [=](int newValue) {
            if (m_updating) return;

            m_updating = true;

            int oldValue = m_previousValues[i];
            int delta = newValue - oldValue;

            if (delta == 0) {
                m_updating = false;
                return;
            }

            std::vector<int> newValues = m_previousValues;
            newValues[i] = newValue;

            int currentSum = std::accumulate(newValues.begin(), newValues.end(), 0);
            int error = currentSum - 360;

            if (error == 0) {
                m_previousValues = newValues;
                m_segmentSliders[i]->setValue(newValue);
                m_updating = false;
                return;
            }

            std::vector<int> indicesToAdjust;
            for (size_t j = 0; j < m_segmentSliders.size(); ++j) {
                if (j != i)
                    indicesToAdjust.push_back(j);
            }

            int remainingError = error;

            int round = 0;
            while (remainingError != 0 && round < 1000) {
                bool adjusted = false;
                for (size_t j : indicesToAdjust) {
                    if (remainingError == 0) break;

                    int current = newValues[j];
                    if (error > 0 && current > 0) {
                        newValues[j] -= 1;
                        remainingError -= 1;
                        adjusted = true;
                    }
                    else if (error < 0 && current < 360) {
                        newValues[j] += 1;
                        remainingError += 1;
                        adjusted = true;
                    }
                }

                if (!adjusted) break;
                ++round;
            }

            if (remainingError != 0) {
                m_segmentSliders[i]->setValue(oldValue); 
                m_updating = false;
                return;
            }

            for (size_t j = 0; j < m_segmentSliders.size(); ++j) {
                m_previousValues[j] = newValues[j];
                m_segmentSliders[j]->setValue(newValues[j]);
            }

            std::vector<float> segmentRatios;
            segmentRatios.reserve(newValues.size());
            for (int val : newValues) {
                segmentRatios.push_back(static_cast<float>(val) / 360.f);
            }
            emit SegmentParameterChanged(segmentRatios);

            m_updating = false;
        });

        m_segmentParametersLayout->addWidget(slider);

    }

    auto resetSegmentsButton = new QPushButton("Reset Segments", this);
    resetSegmentsButton->setStyleSheet("QPushButton { font-size: 12px; }");
    m_segmentParametersLayout->addWidget(resetSegmentsButton);

    connect(resetSegmentsButton, &QPushButton::clicked, this, [=]() {
        m_updating = true;
        m_previousValues.clear();
        for (auto& slider : m_segmentSliders) {
            slider->setValue(360 / m_segmentSliders.size()); 
            m_previousValues.push_back(360 / m_segmentSliders.size());
        }
        std::vector<float> segmentRatios;
        segmentRatios.reserve(m_segmentSliders.size());
        for (int val : m_previousValues) segmentRatios.push_back(static_cast<float>(val) / 360.f);   
        emit SegmentParameterChanged(segmentRatios);
        m_updating = false;
    });

}

void ModelSettingsWidget::Update3DModelParameters(VoxelGrid voxel)
{
    auto layout = qobject_cast<QVBoxLayout*>(m_parameters3DModelGroupBox->layout());
    clearLayout(layout); // Clear previous layout items

    for (unsigned int i = 0; i < voxel.nbTypes; ++i) {
        auto rowLayout = new QHBoxLayout();
        auto checkBox = new QCheckBox();
        checkBox->setStyleSheet("QCheckBox { font-size: 12px; }");
        checkBox->setChecked(true);
        checkBox->setText(QString("Material %1").arg(i));

        connect(checkBox, &QCheckBox::stateChanged, this, [=](int state) {
            if (state == Qt::Checked) {
                for (auto& voxelSpring : voxel.voxelSprings) {
                    if (voxelSpring.materialType == voxel.materialTypes[i]) {
                        for (auto& particle : voxelSpring.corners) {
                            QColor color = particle->GetColor();
                            color.setAlpha(255); // Set alpha to 255 (fully opaque)
                            particle->SetColor(color);
                        }
                        for (auto& spring : voxelSpring.springs) {
                            QColor color = spring->GetColor();
                            color.setAlpha(255); // Set alpha to 255 (fully opaque)
                            spring->SetColor(color);
                        }
                    }
                }
            } else {
                for (auto& voxelSpring : voxel.voxelSprings) {
                    if (voxelSpring.materialType == voxel.materialTypes[i]) {
                        for (auto& particle : voxelSpring.corners) {
                            QColor color = particle->GetColor();
                            color.setAlpha(2); // Set alpha to 2 (fully transparent)
                            particle->SetColor(color);
                        }
                        for (auto& spring : voxelSpring.springs) {
                            QColor color = spring->GetColor();
                            color.setAlpha(2); // Set alpha to 2 (fully transparent)
                            spring->SetColor(color);
                        }
                    }
                }
            }
        });

        rowLayout->addWidget(checkBox);
        m_parameters3DModelLayout->addLayout(rowLayout);
    }
}

void ModelSettingsWidget::SetSizeSlider(int value)
{
    QSignalBlocker blocker(m_sSize);
    m_sSize->setValue(value);
}

void ModelSettingsWidget::SetDeformationSlider(int i1, int i2, int value)
{
    if (i1 == 1 && i2 == 9) {
        QSignalBlocker blocker(m_s1);
        m_s1->setValue(value);
    } else if (i1 == 2 && i2 == 8) {
        QSignalBlocker blocker(m_s2);
        m_s2->setValue(value);
    } else if (i1 == 3 && i2 == 7) {
        QSignalBlocker blocker(m_s3);
        m_s3->setValue(value);
    } else if (i1 == 4 && i2 == 6) {
        QSignalBlocker blocker(m_s4);
        m_s4->setValue(value);
    }
}

void ModelSettingsWidget::SetSamplingModel(int value)
{
    qDebug() << "Setting sampling model to:" << value;
    QSignalBlocker blocker(m_samplingBreastModelSpinBox);
    m_samplingBreastModelSpinBox->setValue(value);
}

void ModelSettingsWidget::SetCurveSize(int value)
{
    qDebug() << "Setting curve size to:" << value;
    QSignalBlocker blocker(m_sSize);
    m_sSize->setValue(value);
}

void ModelSettingsWidget::SetCurveDepth(int value)
{
    qDebug() << "Setting curve depth to:" << value;
    QSignalBlocker blocker(m_sDepth);
    m_sDepth->setValue(value);
}

void ModelSettingsWidget::SetRingRadius(int value)
{
    qDebug() << "Setting ring radius to:" << value;
    QSignalBlocker blocker(m_sRing);
    m_sRing->setValue(value);
}

void ModelSettingsWidget::SetParticleRadiusVolume(int value)
{
    qDebug() << "Setting particle radius volume to:" << value;
    QSignalBlocker blocker(m_sParticleRadiusVolume);
    m_sParticleRadiusVolume->setValue(value);
}

void ModelSettingsWidget::SetSpacingVolume(int value)
{
    qDebug() << "Setting spacing volume to:" << value;
    QSignalBlocker blocker(m_sSpacingVolume);
    m_sSpacingVolume->setValue(value);
}

