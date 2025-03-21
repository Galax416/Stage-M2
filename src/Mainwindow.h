#pragma once

#include <QMainWindow>

#include "OpenglWidget.h"

// Main window class
class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    MainWindow();

private:
    OpenGLWidget* m_openGLWidget;

};