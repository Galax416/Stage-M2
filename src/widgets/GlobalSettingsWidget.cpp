#include "GlobalSettingsWidget.h"
#include "OpenglWidget.h"

GlobalSettingsWidget::GlobalSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(300);
    setMaximumWidth(400);
    InitUI();
    InitConnections();
}

GlobalSettingsWidget::~GlobalSettingsWidget()
{
    
}


void GlobalSettingsWidget::InitUI()
{
    // Set the layout for the widget
    m_mainLayout = new QVBoxLayout(this);
    
    // Global settings group box
    m_globalSettingsGroupBox = new QGroupBox("Global Settings", this);
    m_globalSettingsGroupBox->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");
    m_globalSettingsGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QVBoxLayout* globalSettingsLayout = new QVBoxLayout(m_globalSettingsGroupBox);
    m_globalSettingsGroupBox->setLayout(globalSettingsLayout);

    // Delta time input
    m_dtGroupBox = new QGroupBox("", this);
    m_dtGroupBox->setMaximumHeight(50);
    m_dtGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
    m_dtGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QHBoxLayout* dtLayout = new QHBoxLayout(m_dtGroupBox);
    QLabel* dtLabel = new QLabel("Delta time: ", this);
    dtLabel->setStyleSheet("QLabel { font-size: 12px; }");
    dtLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dtLayout->addWidget(dtLabel);
    m_dtLineEdit = new QLineEdit(this);
    m_dtLineEdit->setValidator(new QDoubleValidator(0.0, 1.0, 4, this));
    m_dtLineEdit->setText(QString::number(DeltaTime, 'f', 4));
    dtLayout->addWidget(m_dtLineEdit);
    m_dtGroupBox->setLayout(dtLayout);
    globalSettingsLayout->addWidget(m_dtGroupBox);

    // Friction slider and spin box
    m_frictionGroupBox = new QGroupBox("Friction", this);
    m_frictionGroupBox->setMaximumHeight(60);
    m_frictionGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
    m_frictionGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QHBoxLayout* frictionLayout = new QHBoxLayout(m_frictionGroupBox);
    m_frictionSlider = new QSlider(Qt::Horizontal, this);
    m_frictionSlider->setRange(0, 100);
    m_frictionSlider->setValue(5);
    m_frictionSlider->setSingleStep(1);
    m_frictionSpinBox = new QSpinBox(this);
    m_frictionSpinBox->setRange(0, 100);
    m_frictionSpinBox->setValue(5);
    m_frictionSpinBox->setSingleStep(1);
    QLabel* frictionLabel = new QLabel("%", this);
    frictionLabel->setStyleSheet("QLabel { font-size: 12px; }");
    frictionLayout->addWidget(m_frictionSlider);
    frictionLayout->addWidget(m_frictionSpinBox);
    frictionLayout->addWidget(frictionLabel);
    m_frictionGroupBox->setLayout(frictionLayout);  
    globalSettingsLayout->addWidget(m_frictionGroupBox);

    // Rotation slider and spin box
    m_rotationGroupBox = new QGroupBox("Rotation", this);
    m_rotationGroupBox->setMaximumHeight(180);
    m_rotationGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
    m_rotationGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QVBoxLayout* rotationLayout = new QVBoxLayout(m_rotationGroupBox);
    // X
    QHBoxLayout* rotationXLayout = new QHBoxLayout();
    QLabel* rotationXLabel = new QLabel("X: ", this);
    rotationXLabel->setStyleSheet("QLabel { font-size: 12px; }");
    m_rotationXSlider = new QSlider(Qt::Horizontal, this);
    m_rotationXSlider->setRange(0, 360);
    m_rotationXSlider->setValue(0);
    m_rotationXSlider->setSingleStep(1);
    m_rotationXSpinBox = new QSpinBox(this);
    m_rotationXSpinBox->setRange(0, 360);
    m_rotationXSpinBox->setValue(0);
    m_rotationXSpinBox->setSingleStep(1);
    QLabel* rotationLabel = new QLabel("°", this);
    rotationLabel->setStyleSheet("QLabel { font-size: 12px; }");
    rotationXLayout->addWidget(rotationXLabel);
    rotationXLayout->addWidget(m_rotationXSlider);
    rotationXLayout->addWidget(m_rotationXSpinBox);
    rotationXLayout->addWidget(rotationLabel);
    rotationLayout->addLayout(rotationXLayout);
    // Y
    QHBoxLayout* rotationYLayout = new QHBoxLayout();
    // rotationLayout->addSpacing(10); // Add spacing between X and Y controls
    QLabel* rotationYLabel = new QLabel("Y: ", this);
    rotationYLabel->setStyleSheet("QLabel { font-size: 12px; }");
    m_rotationYSlider = new QSlider(Qt::Horizontal, this);
    m_rotationYSlider->setRange(0, 360);
    m_rotationYSlider->setValue(0);
    m_rotationYSlider->setSingleStep(1);
    m_rotationYSpinBox = new QSpinBox(this);
    m_rotationYSpinBox->setRange(0, 360);
    m_rotationYSpinBox->setValue(0);
    m_rotationYSpinBox->setSingleStep(1);
    QLabel* rotationYUnitLabel = new QLabel("°", this);
    rotationYUnitLabel->setStyleSheet("QLabel { font-size: 12px; }");
    rotationYLayout->addWidget(rotationYLabel);
    rotationYLayout->addWidget(m_rotationYSlider);
    rotationYLayout->addWidget(m_rotationYSpinBox);
    rotationYLayout->addWidget(rotationYUnitLabel);
    rotationLayout->addLayout(rotationYLayout);
    // Z
    QHBoxLayout* rotationZLayout = new QHBoxLayout();
    // rotationLayout->addSpacing(10); // Add spacing between Y and Z controls
    QLabel* rotationZLabel = new QLabel("Z: ", this);
    rotationZLabel->setStyleSheet("QLabel { font-size: 12px; }");
    m_rotationZSlider = new QSlider(Qt::Horizontal, this);
    m_rotationZSlider->setRange(0, 360);
    m_rotationZSlider->setValue(0);
    m_rotationZSlider->setSingleStep(1);
    m_rotationZSpinBox = new QSpinBox(this);
    m_rotationZSpinBox->setRange(0, 360);
    m_rotationZSpinBox->setValue(0);
    m_rotationZSpinBox->setSingleStep(1);
    QLabel* rotationZUnitLabel = new QLabel("°", this);
    rotationZUnitLabel->setStyleSheet("QLabel { font-size: 12px; }");
    rotationZLayout->addWidget(rotationZLabel);
    rotationZLayout->addWidget(m_rotationZSlider);
    rotationZLayout->addWidget(m_rotationZSpinBox);
    rotationZLayout->addWidget(rotationZUnitLabel);
    rotationLayout->addLayout(rotationZLayout);

    m_rotationGroupBox->setLayout(rotationLayout);
    globalSettingsLayout->addWidget(m_rotationGroupBox);

    // Add void space to the bottom of the group box
    globalSettingsLayout->addStretch(1);



    m_mainLayout->addWidget(m_globalSettingsGroupBox);

}

void GlobalSettingsWidget::InitConnections()
{
    connect(m_frictionSlider, &QSlider::valueChanged, this, [this](int value) { 
        m_frictionSpinBox->setValue(value);
        emit FrictionChanged((float)(value) * 0.01f); // value / 100.0f
    });
    connect(m_frictionSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        m_frictionSlider->setValue(value);
        emit FrictionChanged((float)(value) * 0.01f); // value / 100.0f 
    });

    connect(m_dtLineEdit, &QLineEdit::editingFinished, this, [this]() {
        bool ok;
        float value = m_dtLineEdit->text().toFloat(&ok);
        if (ok) {
            value = qMax(0.0001f, qMin(value, 1.0f)); // Clamp value between 0.0001 and 1.0
            m_dtLineEdit->setText(QString::number(value, 'f', 4));
            emit DeltaTimeChanged(value);
        } else {
            m_dtLineEdit->setText("0.0005");
        }
    });

    connect(m_rotationXSlider, &QSlider::valueChanged, this, [this](int value) {
        m_rotationXSpinBox->setValue(value);
        emit RotationChanged(QVector3D(value, m_rotationYSlider->value(), m_rotationZSlider->value()));
    });
    connect(m_rotationXSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        m_rotationXSlider->setValue(value);
        emit RotationChanged(QVector3D(value, m_rotationYSlider->value(), m_rotationZSlider->value()));
    });

    connect(m_rotationYSlider, &QSlider::valueChanged, this, [this](int value) {
        m_rotationYSpinBox->setValue(value);
        emit RotationChanged(QVector3D(m_rotationXSlider->value(), value, m_rotationZSlider->value()));
    });
    connect(m_rotationYSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        m_rotationYSlider->setValue(value);
        emit RotationChanged(QVector3D(m_rotationXSlider->value(), value, m_rotationZSlider->value()));
    });

    connect(m_rotationZSlider, &QSlider::valueChanged, this, [this](int value) {
        m_rotationZSpinBox->setValue(value);
        emit RotationChanged(QVector3D(m_rotationXSlider->value(), m_rotationYSlider->value(), value));
    });
    connect(m_rotationZSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        m_rotationZSlider->setValue(value);
        emit RotationChanged(QVector3D(m_rotationXSlider->value(), m_rotationYSlider->value(), value));
    });

}