#pragma once

#include <QMainWindow>
#include <QStatusBar>
#include <QMenuBar>
#include <QSplitter>
#include <QWidget>
#include <QVBoxLayout>
#include <QSurfaceFormat>
#include <QScrollArea>
#include <QPushButton>
#include <QShortcut>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include "OpenglWidget.h"
#include "GlobalSettingsWidget.h"
#include "SpringSettingsWidgets.h"
#include "ModelSettingsWidget.h"


// Main window class
class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

public slots:
    void updateStatusBarMessage(const QString& message);
    void updateButtonsState(bool isRunning);

private:
    // QTimer* m_timer;
    
    QStatusBar* m_statusBar;
    QMenuBar*   m_menuBar;

    QSplitter*              m_splitter;
    OpenGLWidget*           m_openGLWidget;
    QWidget*                m_rightContainer;
    QVBoxLayout*            m_rightLayout;
    GlobalSettingsWidget*   m_globalSettingsWidget;
    ModelSettingsWidget*    m_modelSettingsWidget;
    SpringSettingsWidget*   m_springSettingsWidget;


    QPushButton *m_clearButton, *m_resetButton, *m_playStopButton;

};