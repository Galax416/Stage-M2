#include "ModelSettingsWidget.h"

ModelSettingsWidget::ModelSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(300);
    setMaximumWidth(400);
    InitUI();
    InitConnections();
}

ModelSettingsWidget::~ModelSettingsWidget()
{

}

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

    m_createModelButton = new QPushButton("Create Breast Model", this);
    m_createModelButton->setStyleSheet("QPushButton { font-size: 12px; }");
    // m_createModelButton->setVisible(false);
    m_modelSettingsLayout->addWidget(m_createModelButton);

    m_deformModelGroupBox = new QGroupBox("Deform Model", this);
    m_deformModelGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
    m_deformModelGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_deformModelGroupBox->setVisible(false); // Initially hidden

    auto deformModelLayout = new QVBoxLayout(m_deformModelGroupBox);
    auto samplingLayout = new QHBoxLayout();
    auto samplingLabel = new QLabel("Sampling:", this);
    samplingLabel->setStyleSheet("QLabel { font-size: 12px; }");
    samplingLayout->addWidget(samplingLabel);
    m_samplingModelSpinBox = new QSpinBox(this);
    m_samplingModelSpinBox->setStyleSheet("QSpinBox { font-size: 12px; }");
    m_samplingModelSpinBox->setRange(1, 100);
    m_samplingModelSpinBox->setValue(32); // Default value
    m_samplingModelSpinBox->setSingleStep(1);
    m_samplingModelSpinBox->setSuffix(" samples");
    samplingLayout->addWidget(m_samplingModelSpinBox);
    deformModelLayout->addLayout(samplingLayout);

    auto layerLayout = new QHBoxLayout();
    auto layerLabel = new QLabel("Layer:", this);
    layerLabel->setStyleSheet("QLabel { font-size: 12px; }");
    layerLayout->addWidget(layerLabel);
    m_layerModelSpinBox = new QSpinBox(this);
    m_layerModelSpinBox->setStyleSheet("QSpinBox { font-size: 12px; }");
    m_layerModelSpinBox->setRange(1, 100);
    m_layerModelSpinBox->setValue(5); // Default value
    m_layerModelSpinBox->setSingleStep(1);
    m_layerModelSpinBox->setSuffix(" layers");
    layerLayout->addWidget(m_layerModelSpinBox);
    deformModelLayout->addLayout(layerLayout);

    QLabel *deformLabel = new QLabel("Form:", this);
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

    m_s5 = new QSlider(Qt::Horizontal, this);
    m_s5->setRange(0, 100);
    m_s5->setValue(50); // Default value
    m_s5->setSingleStep(1);
    deformModelLayout->addWidget(m_s5);

    QLabel *heightLabel = new QLabel("Height:", this);
    heightLabel->setStyleSheet("QLabel { font-size: 12px; }");
    deformModelLayout->addWidget(heightLabel);

    m_sH = new QSlider(Qt::Horizontal, this);
    m_sH->setRange(0, 100);
    m_sH->setValue(50); // Default value
    m_sH->setSingleStep(1);
    deformModelLayout->addWidget(m_sH);

    QLabel *ringLabel = new QLabel("Areola:", this);
    ringLabel->setStyleSheet("QLabel { font-size: 12px; }");
    deformModelLayout->addWidget(ringLabel);

    m_sRing = new QSlider(Qt::Horizontal, this);
    m_sRing->setRange(0, 100);
    m_sRing->setValue(50); // Default value
    m_sRing->setSingleStep(1);
    deformModelLayout->addWidget(m_sRing);
    
    m_modelSettingsLayout->addWidget(m_deformModelGroupBox);    

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

    connect(m_createModelButton, &QPushButton::clicked, this, [=]() {
        m_createModelButtonClicked = !m_createModelButtonClicked;
        if (m_createModelButtonClicked) {
            m_createModelButton->setText("Destroy Model");
            m_deformModelGroupBox->setVisible(true);
            m_s1->setValue(50); // Reset to default value
            m_s2->setValue(50); // Reset to default value
            m_s3->setValue(50); // Reset to default value
            m_s4->setValue(50); // Reset to default value
            m_s5->setValue(50); // Reset to default value
            m_sH->setValue(50); // Reset to default value
            m_sRing->setValue(50); // Reset to default value
        } else {
            m_createModelButton->setText("Create Model");
            m_deformModelGroupBox->setVisible(false);
        }
        emit CreateModelButtonClicked(m_createModelButtonClicked);
    });

    connect(m_samplingModelSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        emit DeformModelSamplingChanged(value);
    });

    connect(m_layerModelSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        emit DeformModelLayerChanged(value);
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

    connect(m_s5, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelCurveChanged(1, 6, (value * 0.02f) - 1.0f);
    });

    connect(m_sH, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelHeightChanged((value * 0.02f) - 1.0f);
    });

    connect(m_sRing, &QSlider::valueChanged, this, [=](int value) {
        emit DeformModelRingChanged((value * 0.02f) - 1.0f);
    });

}