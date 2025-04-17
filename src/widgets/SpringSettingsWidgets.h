#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QVector3D>
#include <QMap>
#include <QDebug>

class Spring; // Forward declaration of Spring class

class SpringSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpringSettingsWidget(QWidget *parent = nullptr);
    ~SpringSettingsWidget() override;

// signals:

public slots:
    void UpdateSpringsStiffnessControls(const std::vector<std::shared_ptr<Spring>>& springs);

private:
    void InitUI();
    void InitConnections();

    QVBoxLayout* m_mainLayout;

    QGroupBox* m_springSettingsGroupBox;
    QVBoxLayout* m_springSettingsLayout;
    
    QGroupBox* m_stiffnessGroupBox;

};