#include "Mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Set window title
    setWindowTitle("Simulation biomécanique 3D de structures anatomiques déformables par masses-ressorts");

    // Set window size
    //  x    y    w    h
    setGeometry(100, 100, SCREEN_WIDTH, SCREEN_HEIGHT);

    m_openGLWidget = new OpenGLWidget();
    setCentralWidget(m_openGLWidget);

    // Set status bar
    m_statusBar = new QStatusBar();
    m_statusBar->setVisible(false);
    setStatusBar(m_statusBar);
    connect(m_openGLWidget, &OpenGLWidget::statusBarMessageChanged, this, &MainWindow::updateStatusBarMessage);
    
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

void MainWindow::updateStatusBarMessage(const QString& message)
{
    if (message.isEmpty()) {
        m_statusBar->clearMessage();
        m_statusBar->setVisible(false);
        return;
    }
    m_statusBar->setVisible(true);
    m_statusBar->showMessage(message);
}