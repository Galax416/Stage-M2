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
    // Set the layout for the widget
    m_mainLayout = new QVBoxLayout(this);
    
    // Model settings group box
    m_modelSettingsGroupBox = new QGroupBox("Model Settings", this);
    m_modelSettingsGroupBox->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");
    m_modelSettingsGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    // m_modelSettingsGroupBox->setCheckable(true);
    // m_modelSettingsGroupBox->setChecked(false);

    m_modelSettingsLayout = new QVBoxLayout(m_modelSettingsGroupBox);

    m_crossSpringModel = new QCheckBox("Cross Spring", this);
    m_crossSpringModel->setStyleSheet("QCheckBox { font-size: 12px; }");
    m_crossSpringModel->setChecked(true);
    // m_crossSpringModel->setVisible(false);
    m_modelSettingsLayout->addWidget(m_crossSpringModel);

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
            m_thicknessCheckBox = new QCheckBox("Thickness", this);
            m_thicknessCheckBox->setStyleSheet("QCheckBox { font-size: 12px; }");
            m_thicknessCheckBox->setChecked(true);
            deformModelLayout->addWidget(m_thicknessCheckBox);
        }
        
        { // Sampling
            auto samplingLayout = new QHBoxLayout();
            auto samplingLabel = new QLabel("Sampling:", this);
            samplingLabel->setStyleSheet("QLabel { font-size: 12px; }");
            samplingLayout->addWidget(samplingLabel);
            m_samplingBreastModelSpinBox = new QSpinBox(this);
            m_samplingBreastModelSpinBox->setStyleSheet("QSpinBox { font-size: 12px; }");
            m_samplingBreastModelSpinBox->setRange(1, 100);
            m_samplingBreastModelSpinBox->setValue(32); // Default value
            m_samplingBreastModelSpinBox->setSingleStep(1);
            m_samplingBreastModelSpinBox->setSuffix(" samples");
            samplingLayout->addWidget(m_samplingBreastModelSpinBox);
            deformModelLayout->addLayout(samplingLayout);
        }

        { // Layer
            auto layerLayout = new QHBoxLayout();
            auto layerLabel = new QLabel("Layer:", this);
            layerLabel->setStyleSheet("QLabel { font-size: 12px; }");
            layerLayout->addWidget(layerLabel);
            m_layerBreastModelSpinBox = new QSpinBox(this);
            m_layerBreastModelSpinBox->setStyleSheet("QSpinBox { font-size: 12px; }");
            m_layerBreastModelSpinBox->setRange(1, 100);
            m_layerBreastModelSpinBox->setValue(12); // Default value
            m_layerBreastModelSpinBox->setSingleStep(1);
            m_layerBreastModelSpinBox->setSuffix(" layers");
            layerLayout->addWidget(m_layerBreastModelSpinBox);
            deformModelLayout->addLayout(layerLayout);
        }

        { // Width
            QLabel *heightLabel = new QLabel("Width:", this);
            heightLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(heightLabel);
            
            m_sWidth = new QSlider(Qt::Horizontal, this);
            m_sWidth->setRange(0, 100);
            m_sWidth->setValue(47); // Default value
            m_sWidth->setSingleStep(1);
            deformModelLayout->addWidget(m_sWidth);
        }

        { // Height
            QLabel *heightLabel = new QLabel("Height:", this);
            heightLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(heightLabel);
    
            m_sHeight = new QSlider(Qt::Horizontal, this);
            m_sHeight->setRange(0, 100);
            m_sHeight->setValue(57); // Default value
            m_sHeight->setSingleStep(1);
            deformModelLayout->addWidget(m_sHeight);
        }
        
        { // Depth
            QLabel *heightLabel = new QLabel("Depth:", this);
            heightLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(heightLabel);
    
            m_sDepth = new QSlider(Qt::Horizontal, this);
            m_sDepth->setRange(0, 100);
            m_sDepth->setValue(50); // Default value
            m_sDepth->setSingleStep(1);
            deformModelLayout->addWidget(m_sDepth);
        }

        { // Slider form
            auto deformLabel = new QLabel("Form:", this);
            deformLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(deformLabel);
    
            m_s1 = new QSlider(Qt::Horizontal, this);
            m_s1->setRange(0, 100);
            m_s1->setValue(50); // Default value
            m_s1->setSingleStep(1);
            deformModelLayout->addWidget(m_s1);
    
            m_s2 = new QSlider(Qt::Horizontal, this);
            m_s2->setRange(0, 100);
            m_s2->setValue(50); // Default value
            m_s2->setSingleStep(1);
            deformModelLayout->addWidget(m_s2);
    
            m_s3 = new QSlider(Qt::Horizontal, this);
            m_s3->setRange(0, 100);
            m_s3->setValue(50); // Default value
            m_s3->setSingleStep(1);
            deformModelLayout->addWidget(m_s3);
    
            m_s4 = new QSlider(Qt::Horizontal, this);
            m_s4->setRange(0, 100);
            m_s4->setValue(50); // Default value
            m_s4->setSingleStep(1);
            deformModelLayout->addWidget(m_s4);
    
        }

        { // Ring
            QLabel *ringLabel = new QLabel("Areola:", this);
            ringLabel->setStyleSheet("QLabel { font-size: 12px; }");
            deformModelLayout->addWidget(ringLabel);
    
            m_sRing = new QSlider(Qt::Horizontal, this);
            m_sRing->setRange(0, 100);
            m_sRing->setValue(50); // Default value
            m_sRing->setSingleStep(1);
            deformModelLayout->addWidget(m_sRing);
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

    connect(m_crossSpringModel, &QCheckBox::stateChanged, this, [=](int state) {
        emit CrossSpringModelChanged(state == Qt::Checked);
    });

    connect(this, &ModelSettingsWidget::UpdateBreastModelButtonChanged, this, &ModelSettingsWidget::UpdateBreastModelButton);
    connect(this, &ModelSettingsWidget::Update3DModelButtonChanged, this, &ModelSettingsWidget::Update3DModelButton);

    connect(m_createBreastModelButton, &QPushButton::clicked, this, [=]() {
        m_createBreastModelButtonClicked = !m_createBreastModelButtonClicked;

        m_thicknessCheckBox->setChecked(true);
        ResetSliders();

        emit Update3DModelButtonChanged(false); // Hide 3D model parameters
        emit UpdateBreastModelButtonChanged(m_createBreastModelButtonClicked);

    });

    connect(m_thicknessCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
        emit DeformModelThicknessChanged(state == Qt::Checked);
    });

    connect(m_samplingBreastModelSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        emit DeformModelSamplingChanged(value);
    });

    connect(m_layerBreastModelSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        emit DeformModelLayerChanged(value);
    });

    connect(m_sWidth, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelWidthChanged((value * 0.02f) - 1.0f);
    });

    connect(m_sHeight, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelHeightChanged((value * 0.02f) - 1.0f);
    });

    connect(m_sDepth, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelDepthChanged((value * 0.02f) - 1.0f);
    });

    connect(m_s1, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelCurveChanged(2, 10, (value * 0.02f) - 1.0f);
    });

    connect(m_s2, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelCurveChanged(3, 9, (value * 0.02f) - 1.0f);
    });

    connect(m_s3, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelCurveChanged(4, 8, (value * 0.02f) - 1.0f);
    });

    connect(m_s4, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelCurveChanged(5, 7, (value * 0.02f) - 1.0f);
    });

    connect(m_sRing, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelRingChanged((value * 0.02f) - 1.0f);
    });

    connect(m_create3DModelButton, &QPushButton::clicked, this, [=]() {
        m_create3DModelButtonClicked = !m_create3DModelButtonClicked;

        emit UpdateBreastModelButtonChanged(false);
        emit Update3DModelButtonChanged(m_create3DModelButtonClicked);
        
    });

}

void ModelSettingsWidget::ResetSliders()
{
    m_sWidth->setValue(47); // Reset to default value
    m_sHeight->setValue(57); // Reset to default value
    m_sDepth->setValue(50); // Reset to default value
    m_s1->setValue(50); // Reset to default value
    m_s2->setValue(50); // Reset to default value
    m_s3->setValue(50); // Reset to default value
    m_s4->setValue(50); // Reset to default value
    m_sRing->setValue(50); // Reset to default value
}


void ModelSettingsWidget::UpdateBreastModelButton(bool clicked)
{
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