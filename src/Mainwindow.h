#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QShortcut>

#include "OpenglWidget.h"


// Main window class
class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

// protected:
    // void keyPressEvent(QKeyEvent* event) override;

private:
    OpenGLWidget* m_openGLWidget;

};