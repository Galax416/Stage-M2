#include "GlobalSettingsWidget.h"
#include "OpenglWidget.h"

GlobalSettingsWidget::GlobalSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(300);
    setMaximumWidth(400);
    InitUI();
    InitConnections();
}

void GlobalSettingsWidget::InitUI()
{
    // Set the layout for the widget
    m_mainLayout = new QHBoxLayout(this);
    
    // Global settings group box
    m_globalSettingsGroupBox = new QGroupBox("Global Settings", this);
    m_globalSettingsGroupBox->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");
    m_globalSettingsGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto globalSettingsLayout = new QVBoxLayout(m_globalSettingsGroupBox);

    // Delta time input
    m_dtGroupBox = new QGroupBox("", this);
    m_dtGroupBox->setMaximumHeight(50);
    m_dtGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
    m_dtGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    auto dtLayout = new QHBoxLayout(m_dtGroupBox);
    auto dtLabel = new QLabel("Delta time: ", this);
    dtLabel->setStyleSheet("QLabel { font-size: 12px; }");
    dtLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dtLayout->addWidget(dtLabel);
    m_dtLineEdit = new QLineEdit(this);
    m_dtLineEdit->setValidator(new QDoubleValidator(0.0, 1.0, 4, this));
    m_dtLineEdit->setText(QString::number(DELTATIME, 'f', 4));
    dtLayout->addWidget(m_dtLineEdit);
    m_dtButton = new QPushButton("Set", this);
    m_dtButton->setStyleSheet("QPushButton { font-size: 12px; }");
    m_dtButton->setFixedSize(40, 20);
    dtLayout->addWidget(m_dtButton);
    globalSettingsLayout->addWidget(m_dtGroupBox);

    // Friction slider and spin box
    m_frictionGroupBox = new QGroupBox("Friction", this);
    m_frictionGroupBox->setMaximumHeight(60);
    m_frictionGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
    m_frictionGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto frictionLayout = new QHBoxLayout(m_frictionGroupBox);
    m_frictionSlider = new QSlider(Qt::Horizontal, this);
    m_frictionSlider->setRange(0, 100);
    m_frictionSlider->setValue(10);
    m_frictionSlider->setSingleStep(1);
    m_frictionSpinBox = new QSpinBox(this);
    m_frictionSpinBox->setRange(0, 100);
    m_frictionSpinBox->setValue(5);
    m_frictionSpinBox->setSingleStep(1);
    auto frictionLabel = new QLabel("%", this);
    frictionLabel->setStyleSheet("QLabel { font-size: 12px; }");
    frictionLayout->addWidget(m_frictionSlider);
    frictionLayout->addWidget(m_frictionSpinBox);
    frictionLayout->addWidget(frictionLabel);
    globalSettingsLayout->addWidget(m_frictionGroupBox);

    // Background color picker
    m_backgroundColorGroupBox = new QGroupBox("Background Color", this);
    m_backgroundColorGroupBox->setMaximumHeight(60);
    m_backgroundColorGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
    m_backgroundColorGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto backgroundColorLayout = new QHBoxLayout(m_backgroundColorGroupBox);
    auto backgroundColorLabel = new QLabel("Color: ", this);
    backgroundColorLabel->setStyleSheet("QLabel { font-size: 12px; }");
    backgroundColorLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_backgroundColorButton = new QPushButton(this);
    m_backgroundColorButton->setStyleSheet("background-color: rgba(25, 25, 25, 255); border: 1px solid black;");
    m_backgroundColorButton->setFixedSize(40, 20);
    backgroundColorLayout->addWidget(backgroundColorLabel);
    backgroundColorLayout->addWidget(m_backgroundColorButton);
    globalSettingsLayout->addWidget(m_backgroundColorGroupBox);

    /*
    // Rotation slider and spin box
    m_rotationGroupBox = new QGroupBox("Rotation", this);
    m_rotationGroupBox->setMaximumHeight(180);
    m_rotationGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
    m_rotationGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    auto rotationLayout = new QVBoxLayout(m_rotationGroupBox);
    // X
    auto rotationXLayout = new QHBoxLayout();
    auto rotationXLabel = new QLabel("X: ", this);
    rotationXLabel->setStyleSheet("QLabel { font-size: 12px; }");
    m_rotationXSlider = new QSlider(Qt::Horizontal, this);
    m_rotationXSlider->setRange(0, 360);
    m_rotationXSlider->setValue(0);
    m_rotationXSlider->setSingleStep(1);
    m_rotationXSpinBox = new QSpinBox(this);
    m_rotationXSpinBox->setRange(0, 360);
    m_rotationXSpinBox->setValue(0);
    m_rotationXSpinBox->setSingleStep(1);
    auto rotationLabel = new QLabel("°", this);
    rotationLabel->setStyleSheet("QLabel { font-size: 12px; }");
    rotationXLayout->addWidget(rotationXLabel);
    rotationXLayout->addWidget(m_rotationXSlider);
    rotationXLayout->addWidget(m_rotationXSpinBox);
    rotationXLayout->addWidget(rotationLabel);
    rotationLayout->addLayout(rotationXLayout);
    // Y
    auto rotationYLayout = new QHBoxLayout();
    auto rotationYLabel = new QLabel("Y: ", this);
    rotationYLabel->setStyleSheet("QLabel { font-size: 12px; }");
    m_rotationYSlider = new QSlider(Qt::Horizontal, this);
    m_rotationYSlider->setRange(0, 360);
    m_rotationYSlider->setValue(0);
    m_rotationYSlider->setSingleStep(1);
    m_rotationYSpinBox = new QSpinBox(this);
    m_rotationYSpinBox->setRange(0, 360);
    m_rotationYSpinBox->setValue(0);
    m_rotationYSpinBox->setSingleStep(1);
    auto rotationYUnitLabel = new QLabel("°", this);
    rotationYUnitLabel->setStyleSheet("QLabel { font-size: 12px; }");
    rotationYLayout->addWidget(rotationYLabel);
    rotationYLayout->addWidget(m_rotationYSlider);
    rotationYLayout->addWidget(m_rotationYSpinBox);
    rotationYLayout->addWidget(rotationYUnitLabel);
    rotationLayout->addLayout(rotationYLayout);
    // Z
    auto rotationZLayout = new QHBoxLayout();
    auto rotationZLabel = new QLabel("Z: ", this);
    rotationZLabel->setStyleSheet("QLabel { font-size: 12px; }");
    m_rotationZSlider = new QSlider(Qt::Horizontal, this);
    m_rotationZSlider->setRange(0, 360);
    m_rotationZSlider->setValue(0);
    m_rotationZSlider->setSingleStep(1);
    m_rotationZSpinBox = new QSpinBox(this);
    m_rotationZSpinBox->setRange(0, 360);
    m_rotationZSpinBox->setValue(0);
    m_rotationZSpinBox->setSingleStep(1);
    auto rotationZUnitLabel = new QLabel("°", this);
    rotationZUnitLabel->setStyleSheet("QLabel { font-size: 12px; }");
    rotationZLayout->addWidget(rotationZLabel);
    rotationZLayout->addWidget(m_rotationZSlider);
    rotationZLayout->addWidget(m_rotationZSpinBox);
    rotationZLayout->addWidget(rotationZUnitLabel);
    rotationLayout->addLayout(rotationZLayout);

    globalSettingsLayout->addWidget(m_rotationGroupBox);
    */

    // Add void space to the bottom of the group box
    // globalSettingsLayout->addStretch(1);

    m_mainLayout->addWidget(m_globalSettingsGroupBox);

}

void GlobalSettingsWidget::InitConnections()
{

    // Friction
    connect(m_frictionSlider, &QSlider::valueChanged, this, [this](int value) { 
        m_frictionSpinBox->setValue(value);
        emit FrictionChanged((float)(value) * 0.01f); // value / 100.0f
    });
    connect(m_frictionSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        m_frictionSlider->setValue(value);
        emit FrictionChanged((float)(value) * 0.01f); // value / 100.0f 
    });

    // Delta time
    // connect(m_dtLineEdit, &QLineEdit::editingFinished, this, [this]() {
    //     bool ok;
    //     float value = m_dtLineEdit->text().toFloat(&ok);
    //     if (ok) {
    //         value = qMax(0.0001f, qMin(value, 1.0f)); // Clamp value between 0.0001 and 1.0
    //         m_dtLineEdit->setText(QString::number(value, 'f', 4));
    //         emit DeltaTimeChanged(value);
    //     } else {
    //         m_dtLineEdit->setText(QString::number(value, 'f', 4));
    //     }
    // });
    connect(m_dtButton, &QPushButton::clicked, this, [this]() {
        bool ok;
        float value = m_dtLineEdit->text().toFloat(&ok);
        if (ok) {
            value = qMax(0.0001f, qMin(value, 1.0f)); // Clamp value between 0.0001 and 1.0
            m_dtLineEdit->setText(QString::number(value, 'f', 4));
            emit DeltaTimeChanged(value);
        } else {
            m_dtLineEdit->setText(QString::number(value, 'f', 4));
        }
    });
    

    // Background color
    connect(m_backgroundColorButton, &QPushButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(Qt::white, this, "Select Background Color");
        if (color.isValid()) {
            m_backgroundColorButton->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(color.name()));
            emit BackgroundColorChanged(color);
            qDebug() << "Background color changed to:" << color.name();
            qDebug() << "Background color changed to:" << color.red() << color.green() << color.blue();
        }
    });

    /*
    // Rotation
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
    */

}