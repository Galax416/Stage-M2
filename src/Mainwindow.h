#pragma once

#include <QMainWindow>
#include <QStatusBar>
#include <QShortcut>

#include "OpenglWidget.h"


// Main window class
class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void updateStatusBarMessage(const QString& message);

private:
    OpenGLWidget* m_openGLWidget;
    QStatusBar* m_statusBar;

};