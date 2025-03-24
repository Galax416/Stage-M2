#include "Mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Set window title
    setWindowTitle("Simulation biomécanique 3D de structures anatomiques déformables par masses-ressorts");

    // Set window size
    //  x    y    w    h
    setGeometry(100, 100, SCREEN_WIDTH, SCREEN_HEIGHT);
    // resize(SCREEN_WIDTH, SCREEN_HEIGHT);

    m_openGLWidget = new OpenGLWidget();
    setCentralWidget(m_openGLWidget);
    
    // Créer un raccourci clavier pour la touche Tab
    // QShortcut* toggleUIShortcut = new QShortcut(QKeySequence(Qt::Key_Tab), this);
    // connect(toggleUIShortcut, &QShortcut::activated, this, [this]() {
    //     qDebug() << "Tab pressed";
    //     ui->overlayUI->setVisible(!ui->overlayUI->isVisible());
    // });

}

MainWindow::~MainWindow()
{
    delete m_openGLWidget;
}