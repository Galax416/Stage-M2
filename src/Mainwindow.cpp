#include "Mainwindow.h"

MainWindow::MainWindow()
{
    // Set window title
    setWindowTitle("Simulation biomécanique 3D de structures anatomiques déformables par masses-ressorts");

    // Set window size
    resize(SCREEN_WIDTH, SCREEN_HEIGHT);

    m_openGLWidget = new OpenGLWidget(this);

    setCentralWidget(m_openGLWidget);


}