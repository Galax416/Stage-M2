#include "SpringSettingsWidgets.h"
#include "Spring.h"
#include "Utils.h"

#include <memory>

SpringSettingsWidget::SpringSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(300);
    setMaximumWidth(400);
    InitUI();
    InitConnections();
}

// SpringSettingsWidget::~SpringSettingsWidget()
// {

// }

void SpringSettingsWidget::InitUI()
{
    // Set the layout for the widget
    m_mainLayout = new QVBoxLayout(this);
    
    // Spring settings group box
    m_springSettingsGroupBox = new QGroupBox("Spring Settings", this);
    m_springSettingsGroupBox->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");
    m_springSettingsGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_springSettingsLayout = new QVBoxLayout(m_springSettingsGroupBox);

    // Stiffness group box
    m_stiffnessGroupBox = new QGroupBox("Stiffness", this);
    m_stiffnessGroupBox->setStyleSheet("QGroupBox { font-size: 12px; font-weight: normal; }");
    m_stiffnessGroupBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    auto stiffnessLayout = new QVBoxLayout(m_stiffnessGroupBox);
    stiffnessLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_mainLayout->addWidget(m_springSettingsGroupBox);
    
}

void SpringSettingsWidget::InitConnections()
{
    // Connect signals and slots here if needed
}

void SpringSettingsWidget::UpdateSpringsStiffnessControls(const std::vector<std::shared_ptr<Spring>>& springs)
{

    QVBoxLayout* stiffnessLayout = qobject_cast<QVBoxLayout*>(m_stiffnessGroupBox->layout());
    
    clearLayout(stiffnessLayout);

    QMap<float, QVector<std::shared_ptr<Spring>>> kGroups;

    for (const auto& spring : springs) {
        if (spring) {
            kGroups[spring->GetStiffness() * 1e6f].append(spring);
        }
    }

    // Set size of the group box to fit the number of springs
    int numRows = kGroups.size();
    m_stiffnessGroupBox->setMaximumHeight(60 * numRows); 

    // For each group of springs with the same stiffness, we can change the stiffness
    for (auto it = kGroups.begin(); it != kGroups.end(); ++it) {
        float initialK = it.key();
        QVector<std::shared_ptr<Spring>> springGroup = it.value();
        
        auto rowLayout = new QHBoxLayout();

        auto colorLabel = new QLabel;
        colorLabel->setFixedSize(20, 20);
        QPalette palette = colorLabel->palette();
        palette.setColor(QPalette::Window, floatToQColor(initialK));
        colorLabel->setAutoFillBackground(true);
        colorLabel->setPalette(palette);

        auto slider = new QSlider(Qt::Horizontal);
        slider->setRange(1, 1000000);
        slider->setValue(static_cast<int>(initialK));
        slider->setSingleStep(1);

        auto spinBox = new QSpinBox;
        spinBox->setRange(1,  1000000);
        spinBox->setValue(static_cast<int>(initialK));
        spinBox->setSingleStep(1);

        connect(slider, &QSlider::valueChanged, this, [=](int value) {
            spinBox->setValue(value);
            QColor newColor = floatToQColor(value);

            QPalette updatedPalette = palette;
            updatedPalette.setColor(QPalette::Window, newColor);
            colorLabel->setPalette(updatedPalette);
            
            // // Update the spring stiffness
            for (const auto& spring : springGroup) {
                spring->SetStiffness(static_cast<float>(value) / 1.0e6f);
                spring->SetColor(newColor);
            }
        });
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
            slider->setValue(value);
            QColor newColor = floatToQColor(value);

            QPalette updatedPalette = palette;
            updatedPalette.setColor(QPalette::Window, newColor);
            colorLabel->setPalette(updatedPalette);
            
            // // Update the spring stiffness
            for (const auto& spring : springGroup) {
                spring->SetStiffness(static_cast<float>(value) / 1.0e6f);
                spring->SetColor(newColor);
            }
        });

        rowLayout->addWidget(colorLabel);
        rowLayout->addWidget(slider);
        rowLayout->addWidget(spinBox);

        stiffnessLayout->addLayout(rowLayout);
    }

    m_springSettingsLayout->addWidget(m_stiffnessGroupBox);
    
}