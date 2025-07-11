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
    QAction* loadAction = fileMenu->addAction("Load model");
    QAction* saveAction = fileMenu->addAction("Save model");
    fileMenu->addSeparator();
    QAction* loadSceneAction = fileMenu->addAction("Load scene");
    QAction* saveSceneAction = fileMenu->addAction("Save scene");
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("Exit");
    // QMenu* menu2D = m_menuBar->addMenu("2D");
    // QMenu* menu3D = m_menuBar->addMenu("3D");
    m_menuBar->addSeparator();
    QMenu* view = m_menuBar->addMenu("View");
    QAction* vue1 = view->addAction("View 1");
    QAction* vue2 = view->addAction("View 2");
    QAction* vue3 = view->addAction("View 3");
    m_menuBar->addSeparator();
    QMenu* debug = m_menuBar->addMenu("Debug");
    QAction* debugWireframe = debug->addAction("Debug Wireframe");
    QAction* debugColliders = debug->addAction("Debug Colliders");
    QAction* debugBVH = debug->addAction("Debug BVH");
    setMenuBar(m_menuBar);

    // Set splitter
    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->setStretchFactor(0, 7);

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

    // Set OpenGL widget
    m_openGLWidget = new OpenGLWidget();
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(16); // Enable multisampling for anti-aliasing
    m_openGLWidget->setFormat(format);

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
    connect(loadSceneAction, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Load Scene", "", "Scene Files (*.scene);;All Files (*)");
        if (!fileName.isEmpty()) {
            m_openGLWidget->LoadScene(fileName);
        }
    });
    connect(saveSceneAction, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Scene", "", "Scene Files (*.scene);;All Files (*)");
        if (!fileName.isEmpty()) {
            m_openGLWidget->SaveScene(fileName);
        }
    });
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    connect(vue1, &QAction::triggered, this, [this]() {
        m_openGLWidget->SetViewMode(ViewMode::View1);
    });
    connect(vue2, &QAction::triggered, this, [this]() {
        m_openGLWidget->SetViewMode(ViewMode::View2);
    });
    connect(vue3, &QAction::triggered, this, [this]() {
        m_openGLWidget->SetViewMode(ViewMode::View3);
    });
    connect(debugWireframe, &QAction::triggered, this, [this]() {
        m_openGLWidget->SetRenderWireframe();
    });
    connect(debugColliders, &QAction::triggered, this, [this]() {
        m_openGLWidget->SetRenderCollider();
    });
    connect(debugBVH, &QAction::triggered, this, [this]() {
        m_openGLWidget->SetRenderBVH();
    });
    
    // Status bar
    connect(m_openGLWidget, &OpenGLWidget::statusBarMessageChanged, this, &MainWindow::updateStatusBarMessage);
    
    // Gloal settings
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::ClearSceneButtonClicked, m_openGLWidget, &OpenGLWidget::ClearSceneSlot);
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::ClearSceneButtonClicked, m_modelSettingsWidget, &ModelSettingsWidget::ClearSceneSlot);
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::DeltaTimeChanged,        m_openGLWidget, &OpenGLWidget::setGlobalDeltaTime);
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::FrictionChanged,         m_openGLWidget, &OpenGLWidget::setGlobalFriction);
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::BackgroundColorChanged,  m_openGLWidget, &OpenGLWidget::setGlobalBackgroundColor);
    connect(m_globalSettingsWidget, &GlobalSettingsWidget::GravityChanged,          m_openGLWidget, &OpenGLWidget::gravityChanged);

    // Model settings
    connect(m_modelSettingsWidget, &ModelSettingsWidget::CrossSpringModelChanged,                m_openGLWidget, &OpenGLWidget::setCrossSpringModel);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::CreateBreastModelButtonClicked,         m_openGLWidget, &OpenGLWidget::setCurves);
    connect(m_openGLWidget,        &OpenGLWidget::breastSlidersChanged,                          m_modelSettingsWidget, &ModelSettingsWidget::ResetSliders);    
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelThicknessChanged,            m_openGLWidget, &OpenGLWidget::setThickness);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelAttachedChanged,             m_openGLWidget, &OpenGLWidget::setAttached);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelAttachedToModelChanged,      m_openGLWidget, &OpenGLWidget::setAttachedToModel);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::AddParticleButtonClicked,               m_openGLWidget, &OpenGLWidget::addParticle);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelSamplingChanged,             m_openGLWidget, &OpenGLWidget::setSamplingModel);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelLayerChanged,                m_openGLWidget, &OpenGLWidget::setLayerModel);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelCurveChanged,                m_openGLWidget, &OpenGLWidget::setDeformation);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelWidthChanged,                m_openGLWidget, &OpenGLWidget::setCurveWidth);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelHeightChanged,               m_openGLWidget, &OpenGLWidget::setCurveHeight);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelSizeChanged,                 m_openGLWidget, &OpenGLWidget::setCurveSize);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelDepthChanged,                m_openGLWidget, &OpenGLWidget::setCurveDepth);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelRingChanged,                 m_openGLWidget, &OpenGLWidget::setCurveRing);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelSpacingVolumeChanged,        m_openGLWidget, &OpenGLWidget::setSpacingVolume);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::DeformModelParticleRadiusVolumeChanged, m_openGLWidget, &OpenGLWidget::setParticleRadiusVolume);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::UpdateNSegmentsChanged,                 m_openGLWidget, &OpenGLWidget::setNSegements);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::SegmentParameterChanged,                m_openGLWidget, &OpenGLWidget::setSegmentSliders);
    connect(m_modelSettingsWidget, &ModelSettingsWidget::Create3DModelButtonClicked,             m_openGLWidget, &OpenGLWidget::setVoxelModel);

    connect(m_openGLWidget, &OpenGLWidget::setSizeSlider,                 m_modelSettingsWidget, &ModelSettingsWidget::SetSizeSlider);
    connect(m_openGLWidget, &OpenGLWidget::setDeformationSlider,          m_modelSettingsWidget, &ModelSettingsWidget::SetDeformationSlider);
    connect(m_openGLWidget, &OpenGLWidget::setBreastModel,                m_modelSettingsWidget, &ModelSettingsWidget::UpdateBreastModelButton);
    connect(m_openGLWidget, &OpenGLWidget::setSamplingModelSlider,        m_modelSettingsWidget, &ModelSettingsWidget::SetSamplingModel);
    connect(m_openGLWidget, &OpenGLWidget::setCurveSizeSlider,            m_modelSettingsWidget, &ModelSettingsWidget::SetCurveSize);
    connect(m_openGLWidget, &OpenGLWidget::setCurveDepthSlider,           m_modelSettingsWidget, &ModelSettingsWidget::SetCurveDepth);
    connect(m_openGLWidget, &OpenGLWidget::setRingRadiusSlider,           m_modelSettingsWidget, &ModelSettingsWidget::SetRingRadius);
    connect(m_openGLWidget, &OpenGLWidget::setParticleRadiusVolumeSlider, m_modelSettingsWidget, &ModelSettingsWidget::SetParticleRadiusVolume);
    connect(m_openGLWidget, &OpenGLWidget::setSpacingVolumeSlider,        m_modelSettingsWidget, &ModelSettingsWidget::SetSpacingVolume);

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