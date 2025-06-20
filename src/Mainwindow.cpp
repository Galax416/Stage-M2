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
    m_statusBar = new QStatusBar(this);
    m_statusBar->setVisible(false);
    setStatusBar(m_statusBar);

    // Set menu bar
    m_menuBar = new QMenuBar(this);
    QMenu* fileMenu = m_menuBar->addMenu("File");
    QAction* loadAction = fileMenu->addAction("Load");
    QAction* saveAction = fileMenu->addAction("Save");
    // QMenu* menu2D = m_menuBar->addMenu("2D");
    // QMenu* menu3D = m_menuBar->addMenu("3D");
    m_menuBar->addSeparator();
    QMenu* view = m_menuBar->addMenu("View");
    QAction* vue1 = view->addAction("View 1");
    QAction* vue2 = view->addAction("View 2");
    QAction* vue3 = view->addAction("View 3");
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

    auto buttonLayout = new QHBoxLayout();

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
    connect(vue1, &QAction::triggered, this, [this]() {
        m_openGLWidget->SetViewMode(ViewMode::View1);
    });
    connect(vue2, &QAction::triggered, this, [this]() {
        m_openGLWidget->SetViewMode(ViewMode::View2);
    });
    connect(vue3, &QAction::triggered, this, [this]() {
        m_openGLWidget->SetViewMode(ViewMode::View3);
    });
    

    // Status bar
    connect(m_openGLWidget, &OpenGLWidget::statusBarMessageChanged, this, &MainWindow::updateStatusBarMessage);
    
    // Gloal settings
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::DeltaTimeChanged,       m_openGLWidget, &OpenGLWidget::setGlobalDeltaTime);
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::FrictionChanged,        m_openGLWidget, &OpenGLWidget::setGlobalFriction);
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::BackgroundColorChanged, m_openGLWidget, &OpenGLWidget::setGlobalBackgroundColor);

    // Model settings
    connect(m_modelSettingsWidget, &ModelSettingsWidget::CrossSpringModelChanged,        m_openGLWidget, &OpenGLWidget::setCrossSpringModel);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::CreateBreastModelButtonClicked, m_openGLWidget, &OpenGLWidget::setCurves);
    connect(m_openGLWidget,        &OpenGLWidget::breastSlidersChanged,                  m_modelSettingsWidget, &ModelSettingsWidget::ResetSliders);    
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelThicknessChanged,    m_openGLWidget, &OpenGLWidget::setThickness);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelSamplingChanged,     m_openGLWidget, &OpenGLWidget::setSamplingModel);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelLayerChanged,        m_openGLWidget, &OpenGLWidget::setLayerModel);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelCurveChanged,        m_openGLWidget, &OpenGLWidget::setDeformation);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelWidthChanged,        m_openGLWidget, &OpenGLWidget::setCurveWidth);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelHeightChanged,       m_openGLWidget, &OpenGLWidget::setCurveHeight);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelDepthChanged,        m_openGLWidget, &OpenGLWidget::setCurveDepth);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelRingChanged,         m_openGLWidget, &OpenGLWidget::setCurveRing);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::Create3DModelButtonClicked,     m_openGLWidget, &OpenGLWidget::setVoxelModel);

    // Model settings (3D)
    connect(m_openGLWidget, &OpenGLWidget::update3DModelParametersChanged, m_modelSettingsWidget, &ModelSettingsWidget::Update3DModelParameters);

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
        updateButtonsState(m_openGLWidget->IsRunning());
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

void MainWindow::updateButtonsState(bool isRunning)
{
    if (isRunning) {
        m_playStopButton->setText("Stop");
    } else {
        m_playStopButton->setText("Play");
    }
}