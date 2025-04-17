#include "Mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Set window title
    setWindowTitle("Simulation biomécanique 3D de structures anatomiques déformables par masses-ressorts");

    // Set window size
    //  x    y    w    h
    setGeometry(100, 100, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Set Timer
    // m_timer = new QTimer(this);
    // m_timer->setInterval(16); // 60 FPS


    // Set status bar
    m_statusBar = new QStatusBar();
    m_statusBar->setVisible(false);
    setStatusBar(m_statusBar);

    // Set menu bar
    m_menuBar = new QMenuBar(this);
    QMenu* fileMenu = m_menuBar->addMenu("File");
    QAction* loadAction = fileMenu->addAction("Load");
    QAction* saveAction = fileMenu->addAction("Save");
    m_menuBar->addMenu("2D");
    m_menuBar->addMenu("3D");
    setMenuBar(m_menuBar);

    // Set splitter
    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->setStretchFactor(0, 7);

    // Set OpenGL widget
    m_openGLWidget = new OpenGLWidget();
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(16); // Enable multisampling for anti-aliasing
    m_openGLWidget->setFormat(format);
    // m_openGLWidget->setMinimumSize(QSize(1200, 600));

    // Set Right container
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setMaximumWidth(450);
    scrollArea->setMinimumWidth(450);

    m_rightContainer = new QWidget();
    m_rightContainer->setMaximumWidth(450);
    // m_rightContainer->setMinimumWidth(450);
    m_rightLayout = new QVBoxLayout(m_rightContainer);
    m_rightContainer->setLayout(m_rightLayout);

    scrollArea->setWidget(m_rightContainer);

    // Global settings
    m_globalSettingsWidget = new GlobalSettingsWidget(this);
    m_globalSettingsWidget->setMaximumWidth(450);
    m_rightLayout->addWidget(m_globalSettingsWidget);

    // Model settings
    m_modelSettingsWidget = new ModelSettingsWidget(this);
    m_modelSettingsWidget->setMaximumWidth(450);
    m_rightLayout->addWidget(m_modelSettingsWidget);

    // Spring settings
    m_springSettingsWidget = new SpringSettingsWidget(this);
    m_springSettingsWidget->setMaximumWidth(450);
    m_rightLayout->addWidget(m_springSettingsWidget);

    // Add void space to the bottom of the group box
    m_rightLayout->addStretch(1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    // m_clearButton = new QPushButton("Clear", this);
    // buttonLayout->addWidget(m_clearButton);
    m_resetButton = new QPushButton("Reset", this);
    buttonLayout->addWidget(m_resetButton);
    m_playStopButton = new QPushButton("Play", this);
    buttonLayout->addWidget(m_playStopButton);
   
    m_rightLayout->addLayout(buttonLayout);

    


    // Add OpenGL widget and right container to splitter
    m_splitter->addWidget(m_openGLWidget);
    m_splitter->addWidget(scrollArea);

    setCentralWidget(m_splitter);

    // Connections
    // connect(m_timer, &QTimer::timeout, m_openGLWidget, QOverload<>::of(&OpenGLWidget::update));
    // Menu bar
    connect(loadAction, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "OBJ Files (*.obj);;All Files (*)");
        if (!fileName.isEmpty()) {
            m_openGLWidget->LoadOBJ(fileName);
        }
    });
    connect(saveAction, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "OBJ Files (*.obj);;All Files (*)");
        if (!fileName.isEmpty()) {
            m_openGLWidget->SaveOBJ(fileName);
        }
    });

    // Status bar
    connect(m_openGLWidget, &OpenGLWidget::statusBarMessageChanged, this, &MainWindow::updateStatusBarMessage);
    
    // Gloal settings
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::DeltaTimeChanged, m_openGLWidget, &OpenGLWidget::setGlobalDeltaTime);
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::FrictionChanged, m_openGLWidget, &OpenGLWidget::setGlobalFriction);
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::RotationChanged, m_openGLWidget, &OpenGLWidget::setGlobalRotation);

    // Model settings
    connect(m_modelSettingsWidget, &ModelSettingsWidget::CrossSpringModelChanged, m_openGLWidget, &OpenGLWidget::setCrossSpringModel);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::CreateModelButtonClicked, m_openGLWidget, &OpenGLWidget::setCurves);
    connect(m_modelSettingsWidget, QOverload<int>::of(&ModelSettingsWidget::DeformModelChanged), m_openGLWidget, &OpenGLWidget::setSamplingModel);
    connect(m_modelSettingsWidget, QOverload<int, int, float>::of(&ModelSettingsWidget::DeformModelChanged), m_openGLWidget, &OpenGLWidget::setDeformation);

    // Spring settings
    connect(m_openGLWidget, &OpenGLWidget::updateSpringsStiffnessControlsChanged, m_springSettingsWidget, &SpringSettingsWidget::UpdateSpringsStiffnessControls);

    // Buttons (clear, reset, play/stop)
    connect(m_openGLWidget, &OpenGLWidget::buttonStateChanged, this, &MainWindow::updateButtonsState); // Dynamicly change Play to Stop and vice versa
    // connect(m_clearButton, &QPushButton::clicked, this, [this]() {
    //     m_openGLWidget->Clear();
    // });
    connect(m_resetButton, &QPushButton::clicked, this, [this]() {
        m_openGLWidget->Reset();
    });
    connect(m_playStopButton, &QPushButton::clicked, this, [this]() {
        if (m_openGLWidget->IsPaused()) m_openGLWidget->Play();
        else m_openGLWidget->Stop();
        updateButtonsState(m_openGLWidget->IsPaused());
    });


    // Créer un raccourci clavier pour la touche Tab
    // QShortcut* toggleUIShortcut = new QShortcut(QKeySequence(Qt::Key_Tab), this);
    // connect(toggleUIShortcut, &QShortcut::activated, this, [this]() {
    //     qDebug() << "Tab pressed";
    //     ui->overlayUI->setVisible(!ui->overlayUI->isVisible());
    // });

}

MainWindow::~MainWindow()
{
    // if (m_statusBar) delete m_statusBar;
    // if (m_menuBar) delete m_menuBar;
    // if (m_splitter) delete m_splitter;
    // if (m_openGLWidget) delete m_openGLWidget;
    // if (m_rightContainer) delete m_rightContainer;
    // if (m_rightLayout) delete m_rightLayout;
    // if (m_globalSettingsWidget) delete m_globalSettingsWidget;
    // if (m_resetButton) delete m_resetButton;
    // if (m_playStopButton) delete m_playStopButton;
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

void MainWindow::updateButtonsState(bool isPaused)
{
    if (isPaused) {
        m_playStopButton->setText("Play");
    } else {
        m_playStopButton->setText("Stop");
    }
}