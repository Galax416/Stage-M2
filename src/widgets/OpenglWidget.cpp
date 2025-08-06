#include "OpenglWidget.h"
#include "Camera.h"
// #include "Camera.cpp"
#include "Mesh.h" 
#include "Model.h"
#include "CustomOBJLoader.h"
#include "Geometry3D.h"
#include "GeometryPrimitives.h"
#include "ModelPhysicsConverter.h"
#include "PhysicsWorker.h"
#include "Utils.h"
#include "Rigidbody.h"
#include "Render.h"
#include "Remeshing.h"


OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), m_program(0)
{
    setFocusPolicy(Qt::StrongFocus);  // Permit to receive key events
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();
    QMetaObject::invokeMethod(m_worker, "Stop", Qt::QueuedConnection); // Stop the worker thread

    if (m_physicsThread && m_physicsThread->isRunning()) {
        m_physicsThread->quit();
        m_physicsThread->wait();
    }
    doneCurrent();
}

void OpenGLWidget::closeEvent(QCloseEvent *event)
{
    makeCurrent();
    for (auto& rigidbody : m_physicsSystem->GetRigidBodies()) {
        auto particle = std::dynamic_pointer_cast<Particle>(rigidbody);
        if (particle) particle->ReleaseGLResources();

        auto model = std::dynamic_pointer_cast<Model>(rigidbody);
        if (model) model->ReleaseGLResources();
    }
    doneCurrent();
}

QSize OpenGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize OpenGLWidget::sizeHint() const
{
    return QSize(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void OpenGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    if (m_camera) {
        m_camera->SetScreenSize(width, height);
    }
}

void OpenGLWidget::InitShaders(QOpenGLShaderProgram *program, QString vertex_shader, QString geometry_shader, QString fragment_shader) 
{

    if (!vertex_shader.isEmpty()) {
        if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex_shader)) {
            qWarning() << "Error when compiling the geometry shader:" << program->log();
        }
    }

    if (!geometry_shader.isEmpty()) {
        if (!program->addShaderFromSourceFile(QOpenGLShader::Geometry, geometry_shader)) {
            qWarning() << "Error when compiling the geometry shader:" << program->log();
        }
    }

    if (!fragment_shader.isEmpty()) {
        if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment_shader)) {
            qWarning() << "Error when compiling the geometry shader:" << program->log();
        }
    }

    if (!program->link()) {
        qWarning() << "Error when linking the shader program:" << program->log();
        exit(1);
    }
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Initializa shaders programs
    // m_program = new QOpenGLShaderProgram();
    m_program2D = std::make_shared<QOpenGLShaderProgram>();
    m_program3D = std::make_shared<QOpenGLShaderProgram>();

    // 2D shaders
    InitShaders(m_program2D.get(), ":/shaders/2D.vert", "", ":/shaders/2D.frag");
    // 3D shaders
    InitShaders(m_program3D.get(), ":/shaders/3D.vert", "" , ":/shaders/3D.frag");

    if ( m_is2DMode ) m_program = m_program2D;
    else m_program = m_program3D;

    // Initialize the camera
    m_camera = std::make_shared<Camera>(QVector3D(0.0f, 0.0f, 7.0f), QVector3D(0.0f, 0.0f, 0.0f));
    m_camera->SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Initialize the physics system
    m_physicsSystem = std::make_shared<PhysicsSystem>();
    connect(this, &OpenGLWidget::renderColliderChanged, m_physicsSystem.get(), &PhysicsSystem::renderCollider);
    connect(this, &OpenGLWidget::renderBVHChanged, m_physicsSystem.get(), &PhysicsSystem::renderBVH);

    // Initialize the worker
    m_worker = new PhysicsWorker(m_physicsSystem);

    // Initialize the physics thread
    m_physicsThread = std::make_unique<QThread>();
    m_worker->moveToThread(m_physicsThread.get());
    connect(m_physicsThread.get(), &QThread::started, m_worker, &PhysicsWorker::Start);
    connect(m_physicsThread.get(), &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_worker, &PhysicsWorker::Updated, this, QOverload<>::of(&OpenGLWidget::update));
    connect(this, &OpenGLWidget::buttonStateChanged, m_worker, &PhysicsWorker::SetPhysicsRunning);
    connect(this, &OpenGLWidget::deltaTimeChanged, m_worker, &PhysicsWorker::SetDeltaTime);
    m_physicsThread->start();
    
    // Initialize the model
    m_model = std::make_shared<Model>();
    m_model->SetColor(QColor(100, 100, 100));

    // Torso model
    m_torsoModel = std::make_shared<Model>("./resources/models/torso.obj");
    m_torsoModel->SetStatic();
    m_torsoModel->SetColor(QColor(100, 100, 100));
    m_bvhTorsoColliders = BuildBVH(m_torsoModel->triangleColliders);


    InitCurves();
    InitVoxelModel();

    // Initialize stiffness values
    InitStiffnessValues();

    ClearSceneSlot();
    // InitScene();
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);

    glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), m_backgroundColor.alphaF());

    glUseProgram(m_program->programId());


    m_program->bind();
    m_program->setUniformValue("projection", m_camera->GetProjectionMatrix());
    m_program->setUniformValue("view", m_camera->GetViewMatrix());

    m_program->setUniformValue("ligth_position", m_camera->GetPosition());
    m_program->setUniformValue("viewPos", m_camera->GetPosition());
    m_program->release();

    if (m_isWireMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // qint64 currentTime = m_physicsSystem->m_physicsTimer.elapsed();
    // float t = (currentTime - m_physicsSystem->m_lastStepTime) / 1000.0f; // Convert milliseconds to seconds
    // float alpha = t / m_physicsSystem->m_physicsDeltaTime;
    // alpha = std::clamp(alpha, 0.0f, 1.0f); // Ensure alpha is between 0 and 1
    
    m_physicsSystem->Render(m_program.get()/* , alpha */); // Render the physics system

    if (m_mode == ModelMode::ModelModeSurface || m_mode == ModelMode::ModelModeMassSpringAndSurface) UpdateModelFromParticles(m_model);

    // Debug ray
    // m_program->bind();
    // for (const auto& ray : m_debugRays) {
    //     m_program->setUniformValue("material.albedo", QVector3D(1.0f, 0.0f, 0.0f));
    //     m_program->setUniformValue("transparency", 1.0f);
    //     m_program->setUniformValue("model", QMatrix4x4());
    //     ::Render(ray);
    // }
    // m_program->release();

    // Render points for distance measurement
    for (const auto& point : m_distPoints) point->Render(m_program.get());
    

    // if (m_bvhSceneCollider && IsPaused()) RenderBVH(m_program.get(), m_bvhSceneCollider.get());

    
    update();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    // Camera rotation
    m_camera->mousePressEvent(event);

    // If mouse and shift pressed
    if (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)
    {
        bool isRunning = IsRunning();
        Stop(); // Stop the physics system

        // Check if have already two points
        if (m_distPoints.size() >= 2) 
        {
            // Remove points
            for (auto& point : m_distPoints) m_physicsSystem->RemoveRigidbody(point);
            // Clear the vector
            m_distPoints.clear();
        }

        // Get the ray
        Ray ray = ScreenToWorldRay(event->pos(), m_camera.get(), width(), height());
        // qDebug() << "Debug ray origin:" << ray.origin << "direction:" << ray.direction;

        // Get the bvh
        std::vector<std::shared_ptr<TriangleCollider>> triangles;
        if (m_isModel) 
        {
            if (m_model) triangles = m_model->triangleColliders;
        }
        else if (m_isCurve)
        {
            if (m_torsoModel) triangles = m_torsoModel->triangleColliders;
            triangles.insert(triangles.end(), m_triangleColliders.begin(), m_triangleColliders.end());
        }
        else if (m_isVoxelModel)
        {
            
        }
        m_bvhSceneCollider = BuildBVH(triangles);

        // Get the point if is exists
        float minT = std::numeric_limits<float>::max();
        QVector3D point;
        bool hit = false;

        std::vector<std::shared_ptr<TriangleCollider>> closestTriangle;
        QVector3D rayEnd = ray.origin + ray.direction * 500.0f; // Extend the ray to a reasonable length
        QVector3D bbMin = ray.origin;
        QVector3D bbMax = rayEnd;
        AABB rayBox(bbMin, bbMax);
        rayBox.Expand(0.01f);

        QueryBVH<TriangleCollider>(rayBox, m_bvhSceneCollider.get(), closestTriangle);

        for (const auto& triangle : closestTriangle)
        {
            RayCastResult result = RayIntersectsTriangle(ray, triangle);
            if (result.hit && result.t < minT)
            {
                minT = result.t;
                point = result.point;
                hit = true;
            }
        }

        if (hit)
        {
            makeCurrent();
            auto p = std::make_shared<Particle>(point, m_particleRadiusVolume, 1, false, QColor(255, 0, 0));
            m_distPoints.push_back(p);
            doneCurrent(); 
        }
        else m_distPoints.clear();

        if (m_distPoints.size() == 2)
        {
            // Calculate the distance between the two points on the xy plane
            QVector3D p1 = m_distPoints[0]->GetPosition();
            QVector3D p2 = m_distPoints[1]->GetPosition();
            float dist = std::sqrt(std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2));

            // Ref : 6.8 = 61 cm, torso size

            dist = dist * 61.0f / 6.8f; // Convert to cm
            
            emit statusBarMessageChanged(QString("Distance: %1 cm").arg(dist));
        }

        if (isRunning) Play();
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // Camera rotation
    // m_physicsSystem->ChangeGravity(m_camera->GetDownVector() * GRAVITY.length());
    m_camera->mouseReleaseEvent(event);
    // m_debugRay = ScreenToWorldRay(event->pos(), m_camera.get(), width(), height());
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    // Camera rotation
    m_camera->mouseMoveEvent(event);
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    // Camera zoom
    m_camera->wheelEvent(event);
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (Verbose) qDebug() << event->key();
    
    switch (event->key())
    {
    case Qt::Key_Space:
        if (IsPaused()) Play();
        else Stop();
        break;

    case Qt::Key_W:
        m_isWireMode = !m_isWireMode;
        break;

    case Qt::Key_B:
        if (IsPaused()) m_renderBVH = !m_renderBVH;
        emit renderBVHChanged(m_renderBVH);
        break;

    case Qt::Key_C:
        if (Verbose) qDebug() << "Render Collider:" << m_renderCollider;
        m_renderCollider = !m_renderCollider;
        emit renderColliderChanged(m_renderCollider);
        break;

    case Qt::Key_P:
        gravityChanged();
        break;

    case Qt::Key_E:
        addParticle();
        break;

    case Qt::Key_R:
        Stop();
        Reset();
        break;

    case Qt::Key_S:
        {
            bool isRunning = IsRunning();
            Stop();
            m_mode = static_cast<ModelMode>(m_mode + 1 >= 3 ? 0 : m_mode + 1); // Cycle through the modes
            SetModelMode(m_mode);
            emit setModelModeChanged(m_mode);
            if (isRunning) Play();
            break;
        }    

    case 38: // &
        SetViewMode(ViewMode::View1);
        break;
    case 201: // é
        SetViewMode(ViewMode::View2);
        break;
    case 34: // "
        SetViewMode(ViewMode::View3);
        break;
    case 39: // '
        SetViewMode(ViewMode::View4);
        break;
    case 40: // (
        SetViewMode(ViewMode::View5);
        break;
    case 45: // -
        SetViewMode(ViewMode::View6);
        break;

    case 16777236: // Right Arrow
        {
            makeCurrent();
            ChangeControlPointPosition(QVector3D(0.1f, 0.0f, 0.0f));
            Reset();
            doneCurrent();
            break;
        }
    case 16777234: // Left Arrow
        {
            makeCurrent();
            ChangeControlPointPosition(QVector3D(-0.1f, 0.0f, 0.0f));
            Reset();
            doneCurrent();
            break;
        }
    case 16777237: // Down Arrow
        {
            makeCurrent();
            ChangeControlPointPosition(QVector3D(0.0f, -0.1f, 0.0f));
            Reset();
            doneCurrent();
            break;
        }
    case 16777235: // Up Arrow
        {
            makeCurrent();
            ChangeControlPointPosition(QVector3D(0.0f, 0.1f, 0.0f));
            Reset();
            doneCurrent();
            break;
        }

    // case 43: // '+'
    //     {
    //         makeCurrent();
    //         // m_press->SetPosition(m_press->GetPosition() + QVector3D(0, -2, 0) * m_deltaTime);
    //         m_press->SetMass(m_press->GetMass() + 1.0f);
    //         qDebug() << m_press->GetMass();
    //         m_press->SyncCollisionVolumes();
    //         doneCurrent();
    //         break;
    //     }

    // case 45: // '-'
    //     {
    //         makeCurrent();
    //         // m_press->SetPosition(m_press->GetPosition() + QVector3D(0, 2, 0) * m_deltaTime);
    //         m_press->SetMass(m_press->GetMass() - 1.0f);
    //         qDebug() << m_press->GetMass();
    //         m_press->SyncCollisionVolumes();
    //         doneCurrent();
    //         break;
    //     }

    default:
        break;
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    
}

void OpenGLWidget::Reset() 
{
    Stop();
    ClearScene();
    CurveToParticlesSprings();
    VoxelToParticlesSprings();
    InitStiffnessValues();
    InitScene();
}


void OpenGLWidget::InitScene()
{
    // Initialize the scene here
    makeCurrent();    
    
    if (m_isCurve)
    {
        // for (auto& p : m_curve.GetControlPoints()) {
        //     auto particle = std::make_shared<Particle>(p, m_particleRadiusVolume, 1.0f, false, QColor(255,0,0));
        //     m_particles.push_back(particle);
        //     m_physicsSystem->AddRigidbody(particle);
        //     // m_physicsSystem->AddConstraint(particle);
        // }

        ConvertParticleSpringsToModel(m_model, m_particles, m_springs, m_triangleColliders);
        if (m_mode == ModelMode::ModelModeSurface) m_model->Show();
        else if (m_mode == ModelMode::ModelModeMassSpring) m_model->Hide();
        else if (m_mode == ModelMode::ModelModeMassSpringAndSurface) m_model->Show();
        m_physicsSystem->AddRigidbody(m_model);
        // m_physicsSystem->AddConstraint(m_model);

        m_physicsSystem->AddRigidbody(m_torsoModel);
        for (auto& t : m_torsoModel->triangleColliders) m_physicsSystem->AddTriangleCollider(t);

    } 
    else if (m_isVoxelModel) 
    {
        // Create a ground box
        auto ground = std::make_shared<Box>(QVector3D(0, -2, 0), QVector3D(10, 0.2, 10), QColor(150, 150, 150));
        ground->SetStatic();
        m_physicsSystem->AddRigidbody(ground);
        m_physicsSystem->AddConstraint(ground);

        // m_press = std::make_shared<Box>(QVector3D(0, 2, 0), QVector3D(2, 0.2, 2), QColor(200, 200, 200));
        // m_press->SetStatic();
        // m_physicsSystem->AddRigidbody(m_press);
        // m_physicsSystem->AddConstraint(m_press);

        emit update3DModelParametersChanged(m_voxel);

    } 
    else if (m_isModel) 
    {
        // Create a ground box
        // auto ground = std::make_shared<Box>(QVector3D(0, -2, 0), QVector3D(10, 0.2, 10), QColor(150, 150, 150));
        // ground->SetStatic();
        // m_physicsSystem->AddRigidbody(ground);
        // m_physicsSystem->AddConstraint(ground);

        if (!m_model->customOBJ->isCustomOBJ) {
            // Convert the model into particles and springs
            ConvertModelToParticleSprings(m_model, m_particles, m_springs, m_triangleColliders, !m_crossSpringModel);
        } else {
            // Load the model from file
            ChargeModelParticleSprings(m_model, m_particles, m_springs, m_triangleColliders, !m_crossSpringModel);
        }

        m_model->SetDynamic();

        // m_physicsSystem->AddRigidbody(m_model);
        // m_physicsSystem->AddConstraint(m_model);
        
    }

    // qDebug() << "Particles: " << m_particles.size() << " Springs: " << m_springs.size() << " Triangle colliders: " << m_triangleColliders.size();
    
    // Add particles and springs to the physics system
    for (auto& p : m_particles) { m_physicsSystem->AddRigidbody(p); m_physicsSystem->AddConstraint(p); }
    for (auto& s : m_springs) m_physicsSystem->AddSpring(s);
    for (auto& t : m_triangleColliders) m_physicsSystem->AddTriangleCollider(t);

    m_physicsSystem->Update(0.0f); 
    m_physicsSystem->ChangeFriction(m_globalFriction);
    m_physicsSystem->SetUpBVH();
    SetModelMode(m_mode);
    
    emit updateSpringsStiffnessControlsChanged(m_springs);
    
    update();
    doneCurrent();
    
}

void OpenGLWidget::LoadOBJ(const QString& filename)
{
    if (Verbose) qDebug() << "Loading model from file: " << filename;
    
    m_isModel = true;
    m_isCurve = false;
    m_isVoxelModel = false;

    Stop();

    makeCurrent();
    m_model->LoadModel(filename);
    m_model->SetColor(QColor(100, 100, 100));

    // m_model->Remesh(0.2);

    doneCurrent();
    Reset();
}

void OpenGLWidget::SaveOBJ(const QString& filename)
{
    if (Verbose) qDebug() << "Saving model from file: " << filename;
    Stop();
    if (m_isCurve) ConvertParticleSpringsToModel(m_model, m_particles, m_springs, m_triangleColliders);
    m_model->customOBJ->SaveOBJ(filename);
    // emit statusBarMessageChanged("Model saved");
}

void OpenGLWidget::LoadScene(const QString& filename)
{
    if (Verbose) qDebug() << "Loading scene from file: " << filename;
    
    ClearSceneSlot();
    
    // Load all the scene data from a JSON file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open scene file:" << filename;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return;
    }

    QJsonObject sceneRoot = doc.object();

    // Load global parameters
    if (sceneRoot.contains("backgroundColor")) {
        QJsonArray bgColorArray = sceneRoot["backgroundColor"].toArray();
        if (bgColorArray.size() == 4) {
            m_backgroundColor = QColor(bgColorArray[0].toInt(), bgColorArray[1].toInt(), bgColorArray[2].toInt(), bgColorArray[3].toInt());
        }
    }
    if (sceneRoot.contains("deltaTime")) {
        m_deltaTime = static_cast<float>(sceneRoot["deltaTime"].toDouble());
        emit deltaTimeChanged(m_deltaTime);
    }

    // if (sceneRoot.contains("gravity")) {
    //     QJsonArray gravityArray = sceneRoot["gravity"].toArray();
    //     if (gravityArray.size() == 3) {
    //         QVector3D gravity(gravityArray[0].toDouble(), gravityArray[1].toDouble(), gravityArray[2].toDouble());
    //         m_physicsSystem->ChangeGravity(gravity);
    //     }
    // }

    m_isModel = sceneRoot["isModel"].toBool();
    m_isCurve = sceneRoot["isCurve"].toBool();
    m_isVoxelModel = sceneRoot["isVoxel"].toBool();


    if (m_isModel && sceneRoot.contains("modelSettings"))
    {

    }


    if (m_isCurve && sceneRoot.contains("curveSettings"))
    {
        emit setBreastModel(true);

        auto cs = sceneRoot["curveSettings"].toObject();

        if (cs.contains("numSamples")) m_numSamples = cs["numSamples"].toInt();
        if (cs.contains("layers")) m_curveLayers = cs["layers"].toInt();
        if (cs.contains("size")) m_curveSize = cs["size"].toDouble();
        if (cs.contains("lastSize")) m_lastValidCurveSize = cs["lastSize"].toDouble();
        if (cs.contains("depth")) m_curveDepth = cs["depth"].toDouble();
        if (cs.contains("ringRadius")) m_curveRingRadius = cs["ringRadius"].toDouble();
        if (cs.contains("stiffnessAreola")) m_stiffnessAreola = cs["stiffnessAreola"].toDouble();
        if (cs.contains("isAttached")) m_isAttached = cs["isAttached"].toBool();
        if (cs.contains("isAttachedToModel")) m_isAttachedToModel = cs["isAttachedToModel"].toBool();
        if (cs.contains("stiffnessAttached")) m_stiffnessAttached = cs["stiffnessAttached"].toDouble();
        if (cs.contains("spacingVolume")) m_spacingVolume = cs["spacingVolume"].toDouble();
        if (cs.contains("particleRadiusVolume")) m_particleRadiusVolume = cs["particleRadiusVolume"].toDouble();
        if (cs.contains("nSegments")) m_nSegments = cs["nSegments"].toInt();

        if (cs.contains("curveCenter")) {
            QJsonArray arr = cs["curveCenter"].toArray();
            if (arr.size() == 3)
                m_curveCenter = QVector3D(arr[0].toDouble(), arr[1].toDouble(), arr[2].toDouble());
        }
        if (cs.contains("initialCurveCenter")) {
            QJsonArray arr = cs["initialCurveCenter"].toArray();
            if (arr.size() == 3)
                m_initialCurveCenter = QVector3D(arr[0].toDouble(), arr[1].toDouble(), arr[2].toDouble());
        }
        if (cs.contains("initialCurvePoints")) {
            QJsonArray arr = cs["initialCurvePoints"].toArray();
            m_initialCurvePoints.clear();
            for (const auto& v : arr) {
                QJsonArray pt = v.toArray();
                if (pt.size() == 3)
                    m_initialCurvePoints.push_back(QVector3D(pt[0].toDouble(), pt[1].toDouble(), pt[2].toDouble()));
            }
        }
        if (cs.contains("curvePoints")) {
            QJsonArray arr = cs["curvePoints"].toArray();
            m_curvePoints.clear();
            for (const auto& v : arr) {
                QJsonArray pt = v.toArray();
                if (pt.size() == 3)
                    m_curvePoints.push_back(QVector3D(pt[0].toDouble(), pt[1].toDouble(), pt[2].toDouble()));
            }
        }
        if (cs.contains("curvePointsSliders")) {
            QJsonArray arr = cs["curvePointsSliders"].toArray();
            m_curvePointsSliders.clear();
            for (const auto& v : arr) {
                QJsonArray pt = v.toArray();
                if (pt.size() == 3)
                    m_curvePointsSliders.push_back(QVector3D(pt[0].toDouble(), pt[1].toDouble(), pt[2].toDouble()));
            }
        }
        if (cs.contains("defaultCurvePoints")) {
            QJsonArray arr = cs["defaultCurvePoints"].toArray();
            m_defaultCurvePoints.clear();
            for (const auto& v : arr) {
                QJsonArray pt = v.toArray();
                if (pt.size() == 3)
                    m_defaultCurvePoints.push_back(QVector3D(pt[0].toDouble(), pt[1].toDouble(), pt[2].toDouble()));
            }
        }
        if (cs.contains("curveDeformation")) {
            QJsonArray arr = cs["curveDeformation"].toArray();
            m_curveDeformation.clear();
            for (const auto& v : arr) {
                QJsonArray tuple = v.toArray();
                if (tuple.size() == 3)
                    m_curveDeformation.push_back({tuple[0].toInt(), tuple[1].toInt(), tuple[2].toDouble()});
            }
        }
        if (cs.contains("lastDeformation")) {
            QJsonArray arr = cs["lastDeformation"].toArray();
            if (arr.size() == 3)
                m_lastValidDeformation = {arr[0].toInt(), arr[1].toInt(), arr[2].toDouble()};
        }
        if (cs.contains("angularWeights")) {
            QJsonArray arr = cs["angularWeights"].toArray();
            m_angularWeights.clear();
            for (const auto& v : arr) {
                m_angularWeights.push_back(v.toDouble());
            }
        }
        if (cs.contains("stiffnessBySegment")) {
            QJsonArray arr = cs["stiffnessBySegment"].toArray();
            m_stiffnessBySegment.clear();
            for (const auto& v : arr) {
                QJsonObject obj = v.toObject();
                int segment = obj["segment"].toInt();
                double stiffness = obj["stiffness"].toDouble();
                m_stiffnessBySegment[segment] = stiffness;
            }
        }

        emit setSamplingModelSlider(m_numSamples);
        emit setCurveSizeSlider(m_curveSize * 50.0f);
        emit setCurveDepthSlider(m_curveDepth * 50.0f);
        for (const auto& d : m_curveDeformation) emit setDeformationSlider(std::get<0>(d), std::get<1>(d), static_cast<int>((std::get<2>(d) + 1.0f) * 50.f));
        emit setRingRadiusSlider(m_curveRingRadius * 500.0f);
        emit setParticleRadiusVolumeSlider((fromMapped(m_particleRadiusVolume, 4.5f, 9.5f, 6.0) + 1.0f) * 0.5f * 100.0f);
        emit setSpacingVolumeSlider((fromMapped(m_spacingVolume, 0.105f, -0.175f, 0.12f) + 1.0f) * 0.5f * 100.0f);
        emit setAttachedCheckBox(m_isAttached);
        emit setAttachedToModelCheckBox(m_isAttachedToModel);
    }

    if (m_isVoxelModel && sceneRoot.contains("voxelSettings"))
    {
    
    }

    // // Load particles
    // makeCurrent();
    // auto particlesJson = sceneRoot["particles"].toArray();
    // std::vector<std::shared_ptr<Particle>> loadedParticles;
    // for (const auto& item : particlesJson) {
    //     auto obj = item.toObject();

    //     QJsonArray pos = obj["pos"].toArray();
    //     QVector3D position(pos[0].toDouble(), pos[1].toDouble(), pos[2].toDouble());
    //     float radius = obj["radius"].toDouble();
    //     float mass = obj["mass"].toDouble();
    //     bool isDynamic = obj["dynamic"].toBool();

    //     // Create a new particle
    //     auto particle = std::make_shared<Particle>(position, radius * 100.f, mass, isDynamic);
    //     loadedParticles.push_back(particle);
    // }
    // // doneCurrent();

    // // Load springs
    // auto springsJson = sceneRoot["springs"].toArray();
    // std::vector<std::shared_ptr<Spring>> loadedSprings;
    // // makeCurrent();
    // for (const auto& item : springsJson) {
    //     auto obj = item.toObject();

    //     int p1Index = obj["p1"].toInt();
    //     int p2Index = obj["p2"].toInt();
    //     float stiffness = obj["stiffness"].toDouble();

    //     // Create a new spring
    //     auto spring = std::make_shared<Spring>(loadedParticles[p1Index], loadedParticles[p2Index], stiffness);
    //     loadedSprings.push_back(spring);
    // }
    // // doneCurrent();

    // // Load triangle colliders
    // auto triangleCollidersJson = sceneRoot["triangleColliders"].toArray();
    // std::vector<std::shared_ptr<TriangleCollider>> loadedTriangleColliders;
    // // makeCurrent();
    // for (const auto& item : triangleCollidersJson) {
    //     auto obj = item.toObject();
    //     int a = obj["a"].toInt();
    //     int b = obj["b"].toInt();
    //     int c = obj["c"].toInt();
    //     auto triangleCollider = std::make_shared<TriangleCollider>(loadedParticles[a], loadedParticles[b], loadedParticles[c]);
    //     loadedTriangleColliders.push_back(triangleCollider);
    // }
    
    // m_particles = loadedParticles;
    // m_springs = loadedSprings;
    // m_triangleColliders = loadedTriangleColliders;
    // doneCurrent();
    
    // InitScene();
    Reset();
}

void OpenGLWidget::SaveScene(const QString& filename)
{
    if (Verbose) qDebug() << "Saving scene to file: " << filename;
    
    // Save all the scene date in a JSON file
    QJsonObject sceneRoot;

    // Save global parameters
    sceneRoot["backgroundColor"] = QJsonArray{ m_backgroundColor.red(), m_backgroundColor.green(), m_backgroundColor.blue(), m_backgroundColor.alpha() };
    sceneRoot["deltaTime"] = m_deltaTime;
    sceneRoot["gravity"] = QJsonArray{ m_physicsSystem->GetGravity().x(), m_physicsSystem->GetGravity().y(), m_physicsSystem->GetGravity().z() };
    sceneRoot["isModel"] = m_isModel;
    sceneRoot["isCurve"] = m_isCurve;
    sceneRoot["isVoxelModel"] = m_isVoxelModel;

    // Save model settings
    if (m_isModel)
    {

    }
    // Save curve settings
    else if (m_isCurve)
    {
        QJsonObject curveObject;

        QJsonArray initialCurvePointsArray;
        for (const auto& pt : m_initialCurvePoints) initialCurvePointsArray.append(QJsonArray{ pt.x(), pt.y(), pt.z() });
        curveObject["initialCurvePoints"] = initialCurvePointsArray;

        QJsonArray curvePointsArray;
        for (const auto& pt : m_curvePoints) curvePointsArray.append(QJsonArray{ pt.x(), pt.y(), pt.z() });
        curveObject["curvePoints"] = curvePointsArray;

        QJsonArray curvePointsSlidersArray;
        for (const auto& pt : m_curvePointsSliders) curvePointsSlidersArray.append(QJsonArray{ pt.x(), pt.y(), pt.z() });
        curveObject["curvePointsSliders"] = curvePointsSlidersArray;

        QJsonArray defaultCurvePointsArray;
        for (const auto& pt : m_defaultCurvePoints) defaultCurvePointsArray.append(QJsonArray{ pt.x(), pt.y(), pt.z() });
        curveObject["defaultCurvePoints"] = defaultCurvePointsArray;

        QJsonArray curveDeformationArray;
        for (const auto& deformation : m_curveDeformation) curveDeformationArray.append(QJsonArray{ std::get<0>(deformation), std::get<1>(deformation), std::get<2>(deformation) });
        curveObject["curveDeformation"] = curveDeformationArray;
        curveObject["lastDeformation"] = QJsonArray{ std::get<0>(m_lastValidDeformation), std::get<1>(m_lastValidDeformation), std::get<2>(m_lastValidDeformation) };

        curveObject["curveCenter"] = QJsonArray{ m_curveCenter.x(), m_curveCenter.y(), m_curveCenter.z() };
        curveObject["initialCurveCenter"] = QJsonArray{ m_initialCurveCenter.x(), m_initialCurveCenter.y(), m_initialCurveCenter.z() };
        curveObject["numSamples"] = m_numSamples;
        curveObject["layers"] = m_curveLayers;
        curveObject["size"] = m_curveSize;
        curveObject["lastSize"] = m_lastValidCurveSize;
        curveObject["depth"] = m_curveDepth;
        curveObject["ringRadius"] = m_curveRingRadius;
        curveObject["stiffnessAreola"] = m_stiffnessAreola;
        // curveObject["haveThickness"] = m_haveThickness;
        curveObject["isAttached"] = m_isAttached;
        curveObject["isAttachedToModel"] = m_isAttachedToModel;
        curveObject["stiffnessAttached"] = m_stiffnessAttached;
        curveObject["spacingVolume"] = m_spacingVolume;
        curveObject["particleRadiusVolume"] = m_particleRadiusVolume;
        curveObject["nSegments"] = m_nSegments;

        QJsonArray stiffnessBySegment;
        for (const auto& stiffness : m_stiffnessBySegment) {
            QJsonObject obj;
            obj["segment"] = stiffness.first;
            obj["stiffness"] = stiffness.second;
            stiffnessBySegment.append(obj);
        }
        curveObject["stiffnessBySegment"] = stiffnessBySegment;

        QJsonArray angularWeightsArray;
        for (const auto& weight : m_angularWeights) angularWeightsArray.append(weight);
        curveObject["angularWeights"] = angularWeightsArray;

        sceneRoot["curveSettings"] = curveObject;
    }
    // Save voxel model settings
    else if (m_isVoxelModel)
    {

    }


    // // Save the particles
    // QJsonArray particlesArray;
    // for (const auto& particle : m_particles) 
    // {
    //     QJsonObject particleObject;
    //     particleObject["pos"] = QJsonArray{particle->GetPosition().x(), particle->GetPosition().y(), particle->GetPosition().z()};
    //     particleObject["radius"] = particle->GetRadius();
    //     particleObject["mass"] = particle->GetMass();
    //     particleObject["dynamic"] = particle->IsDynamic();
    //     // particleObject["flags"] = particle->GetFlags();
    //     particlesArray.append(particleObject);
    // }
    // sceneRoot["particles"] = particlesArray;

    // // Save the springs
    // QJsonArray springsArray;
    // for (const auto& spring : m_springs)
    // {
    //     QJsonObject springObject;
    //     int a = static_cast<int>(std::distance(m_particles.begin(), std::find(m_particles.begin(), m_particles.end(), spring->GetP1())));
    //     int b = static_cast<int>(std::distance(m_particles.begin(), std::find(m_particles.begin(), m_particles.end(), spring->GetP2())));
        
    //     springObject["p1"] = a;
    //     springObject["p2"] = b;
    //     springObject["stiffness"] = spring->GetStiffness();
    //     springsArray.append(springObject);
    // }
    // sceneRoot["springs"] = springsArray;

    // // Save the triangle colliders
    // QJsonArray triangleCollidersArray;
    // for (const auto& triangle : m_triangleColliders)
    // {
    //     QJsonObject triangleObject;
    //     int a = static_cast<int>(std::distance(m_particles.begin(), std::find(m_particles.begin(), m_particles.end(), triangle->p0)));
    //     int b = static_cast<int>(std::distance(m_particles.begin(), std::find(m_particles.begin(), m_particles.end(), triangle->p1)));
    //     int c = static_cast<int>(std::distance(m_particles.begin(), std::find(m_particles.begin(), m_particles.end(), triangle->p2)));
    //     triangleObject["a"] = a;
    //     triangleObject["b"] = b;
    //     triangleObject["c"] = c;
    //     triangleCollidersArray.append(triangleObject);
    // }
    // sceneRoot["triangleColliders"] = triangleCollidersArray;

    // Write the scene to a JSON file
    QJsonDocument sceneDoc(sceneRoot);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to write scene file:" << file.errorString();
        return;
    }
    file.write(sceneDoc.toJson(QJsonDocument::Indented));
    file.close();

}

void OpenGLWidget::InitCurves()
{
    QVector3D torsoSize = m_torsoModel->GetAABB().size;
    float torsoHeight = torsoSize.z() + 0.1f; // Add a small offset to avoid z-fighting with the ground

    // Curve
    m_curve.SetCurveType(CurveType::BSpline);
    m_curve.Clear();

    // Default control points for the curve
    m_curve.AddControlPoint(QVector3D(0, 1, torsoHeight));
    m_curve.AddControlPoint(QVector3D(-0.38, 0.92, torsoHeight));
    m_curve.AddControlPoint(QVector3D(-0.70, 0.70, torsoHeight));
    m_curve.AddControlPoint(QVector3D(-1, 0, torsoHeight));
    m_curve.AddControlPoint(QVector3D(-0.70, -0.70, torsoHeight));
    m_curve.AddControlPoint(QVector3D(0, -1, torsoHeight));
    m_curve.AddControlPoint(QVector3D(0.70, -0.70, torsoHeight));
    m_curve.AddControlPoint(QVector3D(1, 0, torsoHeight));
    m_curve.AddControlPoint(QVector3D(0.71, 0.71, torsoHeight));
    m_curve.AddControlPoint(QVector3D(0.38, 0.92, torsoHeight));
    
    m_initialCurvePoints = m_curve.GetControlPoints();
    m_curvePoints = m_initialCurvePoints;
    m_curvePointsSliders = m_initialCurvePoints;
    
    // Change position and size of the curve
    UpdateCurveHeightWidth();
    ChangeControlPointPosition(QVector3D(1.0, 1.1, 0));

    m_initialCurveCenter = m_curve.GetCenter();
    m_curveCenter = m_initialCurveCenter;

    m_defaultCurvePoints = m_curve.GetControlPoints();

    size_t n = m_curvePoints.size() * 0.5f;
    for (size_t i = 1; i < n ; ++i) m_curveDeformation.push_back({i, m_curvePoints.size() - i, 0.0f});

    m_stiffnessBySegment.clear();
    switch ( m_nSegments )
    {
        case 1:
            m_stiffnessBySegment[0] = 400.0f;
            break;
        case 2:
            m_stiffnessBySegment[0] = 400.0f;
            m_stiffnessBySegment[1] = 410.0f;
            break;
        case 3:
            m_stiffnessBySegment[0] = 400.0f;
            m_stiffnessBySegment[1] = 450.0f;
            m_stiffnessBySegment[2] = 410.0f;
            break;
        case 4:
            m_stiffnessBySegment[0] = 400.0f;
            m_stiffnessBySegment[1] = 450.0f;
            m_stiffnessBySegment[2] = 410.0f;
            m_stiffnessBySegment[3] = 4550.0f;
            break;
    }

}

void OpenGLWidget::ChangeControlPointPosition(const QVector3D& direction)
{    
    bool isValid = true;
    for (auto& pt : m_curvePoints)
    {
        QVector3D p = QVector3D(pt.x() + direction.x(), pt.y() + direction.y(), pt.z() + direction.z());
        if (!GetPointOntoMesh(p)) 
        {
            isValid = false;
            break;
        }
    }

    if (isValid)
    {
        for (size_t i = 0; i < m_curvePoints.size(); ++i)
        {
            m_initialCurvePoints[i] += direction;
            m_curvePointsSliders[i] += direction;
                                                
            QVector3D p = m_curvePoints[i] + direction;
            m_curve.SetControlPoint(i, p);

        }
    }

    m_curvePoints = m_curve.GetControlPoints();

}

bool OpenGLWidget::GetPointOntoMesh(QVector3D& point)
{
    // Project the point onto the mesh
    RayCastResult result;
    bool hit = false;

    Ray ray(point, QVector3D(0, 0, -1));

    float minT = std::numeric_limits<float>::max();

    std::vector<std::shared_ptr<TriangleCollider>> triangles;
    QueryBVH<TriangleCollider>(AABB(point - QVector3D(0.01,0.01,0.01), point + QVector3D(0.01,0.01,0.01)), m_bvhTorsoColliders.get(), triangles);

    for (const auto& triangle : triangles)
    {
        result = RayIntersectsTriangle(ray, triangle);
        if (result.hit && result.t < minT)
        {
            minT = result.t;
            point = result.point;
            hit = true;
        }
    }

    return hit;
}

void OpenGLWidget::BuildBreast(const std::vector<QVector3D>& profile, const std::unordered_map<int, float>& stiffnessBySegment, bool isMirrored)
{
    makeCurrent();

    // Parameters
    float mass = 1.0f;
    int numLayers = m_curveLayers + 2;
    float layerStep = m_curveDepth / (numLayers - 1);
    float curveRingRadius = m_curveRingRadius;
    int numSamples = m_numSamples; 
    float stiffnessInterLayer = m_stiffnessInterLayer;
    float stiffnessAreola = m_stiffnessAreola;
    float thickness = 0.1f;

    std::vector<std::shared_ptr<Particle>> particles;
    std::vector<std::shared_ptr<Spring>> springs;
    std::vector<std::shared_ptr<TriangleCollider>> triangleColliders;

    size_t numPoints = profile.size();
    if (numPoints < 3) return; // Not enough profilePoints to create a curve

    // Compute the normal of the curve
    QVector3D curveNormal(0, 0, 0);
    for (size_t i = 0; i < numPoints; ++i)
    {
        auto& p1 = profile[i];
        auto& p2 = profile[(i + 1) % numPoints];
        auto& p3 = profile[(i + 2) % numPoints];

        QVector3D v1 = p2 - p1;
        QVector3D v2 = p3 - p2;

        curveNormal += QVector3D::crossProduct(v1, v2).normalized();
    }
    curveNormal.normalize();

    // Get the center of the curves
    QVector3D centerCurve = QVector3D(0, 0, 0);
    for (const auto& pt : profile) centerCurve += pt;
    centerCurve /= static_cast<float>(numPoints);

    // m_curveCenter = centerCurve;
    m_posToAddParticle.push_back(centerCurve);
    QVector3D centerRing = centerCurve; 

    // Add offset to the center
    // centerRing.setY(centerRing.y() - 0.2f);
    // Set the center to the height
    // centerRing += curveNormal * height;:

    QVector3D zAxis = curveNormal;
    QVector3D xAxis;

    if (std::abs(zAxis.y()) < 0.99f) xAxis = QVector3D::crossProduct(zAxis, QVector3D(0, 1, 0)).normalized();
    else xAxis = QVector3D::crossProduct(zAxis, QVector3D(1, 0, 0)).normalized();

    QVector3D yAxis = QVector3D::crossProduct(zAxis, xAxis).normalized();

    // Ring curve
    std::vector<QVector3D> ringPoints;
    for (size_t i = 0; i < numPoints; ++i)
    {
        float angle = static_cast<float>(i) / static_cast<float>(numPoints) * 2.0f * M_PI - M_PI_4;

        float cosAngle = std::cos(angle);
        float sinAngle = std::sin(angle);
        QVector3D p = cosAngle * xAxis * curveRingRadius + sinAngle * yAxis * curveRingRadius;

        ringPoints.push_back(centerRing + p * 1.5f);
    }    

    // Create the layers of particles
    std::vector<std::vector<std::shared_ptr<Particle>>> layers;

    for (int layer = 0; layer < numLayers; ++layer)
    {
        std::vector<std::shared_ptr<Particle>> layerParticles;

        float t = static_cast<float>(layer) / static_cast<float>(numLayers - 1);
        float tCurve = std::pow(t, 3.0f); //1.0f - std::sqrt(1.0f - t * t); // 1.0f + std::sin(-(1.0f - t) * M_PI_2); 
        float z = layer * layerStep;

        // float previousStiffness = -1.0f;

        for (size_t i = 0; i < numPoints; ++i) {
            QVector3D pos = (1.0f - tCurve) * profile[i] + tCurve * ringPoints[i];
            pos.setZ(pos.z() + z);

            auto p = std::make_shared<Particle>(pos, 1, mass, (layer != 0));
            if (layer == 0) p->SetIsConstraint(true);
            layerParticles.push_back(p);
            particles.push_back(p);
        }

        layers.push_back(layerParticles);
    }

    centerRing.setZ(centerRing.z() + m_curveDepth);

    // Center particle
    auto centerParticle = std::make_shared<Particle>(centerRing, 1, mass);
    centerParticle->SetFlags(ParticleFlags::PARTICLE_BORDER);
    centerParticle->SetFlags(ParticleFlags::PARTICLE_CENTER);

    auto ringlayers = layers.back();
    for (size_t i = 0; i < ringlayers.size(); ++i) {
        auto& p1 = ringlayers[i];
        if (i % 4 == 0) p1->SetIsConstraint(true);
    }

    // Triangle collider generation
    for (int layer = 0; layer < numLayers - 1; ++layer)
    {
        const auto& current = layers[layer];
        const auto& next = layers[layer + 1];
        size_t count = current.size();

        for (size_t i = 0; i < count; ++i)
        {
            size_t next_i = (i + 1) % count;

            // First triangle of the quad
            auto a = current[i];
            auto b = next[i];
            auto c = next[next_i];

            // Second triangle of the quad
            auto d = current[next_i];

            if (isMirrored)
            {
                triangleColliders.push_back(std::make_shared<TriangleCollider>(a, b, c));
                triangleColliders.push_back(std::make_shared<TriangleCollider>(c, d, a));
            }
            else
            {
                triangleColliders.push_back(std::make_shared<TriangleCollider>(b, a, c));
                triangleColliders.push_back(std::make_shared<TriangleCollider>(c, a, d));
            }

        }
    }

    // 1. Create a mapping from vertices to particles
    std::unordered_map<SurfaceMesh::Vertex_index, std::shared_ptr<Particle>> vertexToParticleMap;
    RemeshingConstraints constraints;

    // 2. Convert particles to CGAL mesh with constraints
    SurfaceMesh mesh = ConvertToCGALMeshWithConstraints(particles, triangleColliders, vertexToParticleMap, constraints);

    // 3. Remeshing with constraints
    double target_edge_length = 360.0f / numSamples * 0.02f + 0.01f;
    RemeshWithConstraints(mesh, constraints, target_edge_length, 5);

    auto segmentMap = AssignSegmentToVertices(mesh, centerRing, m_angularWeights, QVector3D(0, 0, 1), m_nSegments == 2 ? QVector3D(1, 0, 0) : QVector3D(0, 1, 0));

    particles.clear();
    springs.clear();
    triangleColliders.clear();

    ReconstructFromCGALMesh(mesh, segmentMap, stiffnessBySegment, vertexToParticleMap, particles, springs, triangleColliders);

    auto borderSprings = GetBorderSprings(mesh, vertexToParticleMap, springs);
    if (borderSprings.size() >= 2) {
        BorderSprings& profil = borderSprings[0];
        BorderSprings& ring   = borderSprings[1];

        for (auto& p : profil.particles) { p->SetStatic(); p->SetFlags(ParticleFlags::PARTICLE_BORDER); }
        for (auto& s : profil.springs) { s->SetStiffness(stiffnessAreola); s->SetColor(floatToQColor(stiffnessAreola)); }
        for (auto& p : ring.particles) { p->SetFlags(ParticleFlags::PARTICLE_BORDER); }
        for (auto& s : ring.springs) { s->SetStiffness(stiffnessAreola); s->SetColor(floatToQColor(stiffnessAreola)); }

        particles.push_back(centerParticle);
        
        // Copy unordered_set to vector for indexed access
        size_t ringSize = ring.particles.size();
        for (size_t i = 0; i < ringSize; ++i)
        {
            // Springs connections
            auto s = std::make_shared<Spring>(ring.particles[i], centerParticle, stiffnessAreola);
            springs.push_back(s);

            // Triangles Colliders
            size_t next_i = (i + 1) % ringSize;

            auto a = ring.particles[i];
            auto b = ring.particles[next_i];
            auto c = centerParticle;

            triangleColliders.push_back(std::make_shared<TriangleCollider>(a, b, c));
        }
    }

    std::vector<std::shared_ptr<TriangleCollider>> fillTriangleColliders = triangleColliders;

    // Add a closed triangle 
    const QVector3D& center = centerCurve + (isMirrored ? -curveNormal : curveNormal) * 0.2f; // Offset the center a bit
    for (size_t i = 0; i < numPoints; ++i) {
        const QVector3D& a = profile[i];
        const QVector3D& b = profile[(i + 1) % numPoints]; // wrap around

        // Triangle (a, b, center)
        auto tri = std::make_shared<TriangleCollider>(a, b, center);
        fillTriangleColliders.push_back(tri);
    }

    doneCurrent();

    FillVolumeWithParticle(profile, fillTriangleColliders);
    if (m_haveThickness) Add2ndLayer(particles, springs, triangleColliders, centerCurve, thickness, stiffnessInterLayer, stiffnessAreola);

    m_particles.insert(m_particles.end(), particles.begin(), particles.end());
    m_springs.insert(m_springs.end(), springs.begin(), springs.end());
    m_triangleColliders.insert(m_triangleColliders.end(), triangleColliders.begin(), triangleColliders.end());
    
}

void OpenGLWidget::FillVolumeWithParticle(const std::vector<QVector3D>& profile, std::vector<std::shared_ptr<TriangleCollider>>& triangleColliders)
{
    if (!m_isCurve) return;

    makeCurrent();

    float particleMass = 1.0f;
    float particleRadius = m_particleRadiusVolume;
    float spacing = m_spacingVolume;
    float stiffness = m_stiffnessAttached;

    std::unique_ptr<BVHNode<TriangleCollider>> bvh = BuildBVH<TriangleCollider>(triangleColliders);

    QVector3D minBound = profile[0];
    QVector3D maxBound = profile[0];

    // Compute the bounding box of the curve
    for (const auto& pt : profile) {
        minBound.setX(std::min(minBound.x(), pt.x()));
        minBound.setY(std::min(minBound.y(), pt.y()));

        maxBound.setX(std::max(maxBound.x(), pt.x()));
        maxBound.setY(std::max(maxBound.y(), pt.y()));
    }

    minBound.setZ(minBound.z() + particleRadius * 0.01f);
    maxBound.setZ(maxBound.z() + (m_curveDepth * m_curveNormal).z() * 1.5f + particleRadius * 0.01f);

    int countX = static_cast<int>((maxBound.x() - minBound.x()) / spacing) + 1;
    int countY = static_cast<int>((maxBound.y() - minBound.y()) / spacing) + 1;
    int countZ = static_cast<int>((maxBound.z() - minBound.z()) / spacing) + 1;

    auto getIndex = [&](int x, int y, int z) {
        return x + y * countX + z * countX * countY;
    };

    std::vector<std::shared_ptr<Particle>> particlesNearToTorso;
    bool isNearToTorso = false;
    std::vector<std::shared_ptr<Particle>> temp(countX * countY * countZ, nullptr);

    for (int i = 0; i < countX; ++i) {
        for (int j = 0; j < countY; ++j) {
            for (int k = 0; k < countZ; ++k) {
                QVector3D pos(minBound.x() + i * spacing,
                          minBound.y() + j * spacing,
                          minBound.z() + k * spacing);

                auto p = std::make_shared<Particle>(pos, particleRadius, particleMass);

                std::vector<std::shared_ptr<TriangleCollider>> trianglesClosest;
                // QueryBVH<TriangleCollider>(p->GetAABB(), bvh.get(), trianglesClosest);
                QueryBVH<TriangleCollider>(FromRay(Ray(p->GetPosition(), QVector3D(0, 0, 1)), 0.001f), bvh.get(), trianglesClosest);
                
                bool isInside = IsParticleInsideMesh(p, trianglesClosest);

                if (isInside && !isNearToTorso) { 
                    particlesNearToTorso.push_back(p); 
                    isNearToTorso = true; 
                }
                if (isInside) temp[getIndex(i, j, k)] = p;
                
            }
            isNearToTorso = false;
        }
        isNearToTorso = false;
    }

    std::vector<std::shared_ptr<Particle>> particlesToAdd;
    
    for (int i = 0; i < countX; ++i) {
        for (int j = 0; j < countY; ++j) {
            for (int k = 0; k < countZ; ++k) {
                auto p = temp[getIndex(i, j, k)];
                if (!p) continue;
                
                if (m_isAttached)
                {
                    // X+1
                    if (i + 1 < countX) {
                        auto neighbor = temp[getIndex(i + 1, j, k)];
                        if (neighbor) m_springs.push_back(std::make_shared<Spring>(p, neighbor, stiffness));
                    }
                    // Y+1
                    if (j + 1 < countY) {
                        auto neighbor = temp[getIndex(i, j + 1, k)];
                        if (neighbor) m_springs.push_back(std::make_shared<Spring>(p, neighbor, stiffness));
                    }
                    // Z+1
                    if (k + 1 < countZ) {
                        auto neighbor = temp[getIndex(i, j, k + 1)];
                        if (neighbor) m_springs.push_back(std::make_shared<Spring>(p, neighbor, stiffness));
                    }
                }

                particlesToAdd.push_back(p);
                m_particles.push_back(p);
            }
        }
    }

    if (m_isAttached)
    {
        if (m_isAttachedToModel) 
        {
            float threshold = spacing * 1.1f;
    
            for (auto& p : particlesToAdd) {
                std::vector<std::shared_ptr<TriangleCollider>> trianglesClosest;
                QueryBVH<TriangleCollider>(FromRay(Ray(p->GetPosition(), QVector3D(0, 0, 1)), 0.001f), bvh.get(), trianglesClosest);
    
                float minDist = std::numeric_limits<float>::max();
                std::shared_ptr<TriangleCollider> closestTriangle = nullptr;
    
                for (auto& tri : trianglesClosest) {
                    float d = tri->DistanceTo(p->GetPosition());
                    if (d < minDist) {
                        minDist = d;
                        closestTriangle = tri;
                    }
                }
    
                if (closestTriangle && minDist < threshold) {
                    if (!closestTriangle->p0 || !closestTriangle->p1 || !closestTriangle->p2) continue;
                    // auto p0 = closestTriangle->p0 ? closestTriangle->p0 : std::make_shared<Particle>(closestTriangle->pos0, 1, 1.0f);
                    // auto p1 = closestTriangle->p1 ? closestTriangle->p1 : std::make_shared<Particle>(closestTriangle->pos1, 1, 1.0f);
                    // auto p2 = closestTriangle->p2 ? closestTriangle->p2 : std::make_shared<Particle>(closestTriangle->pos2, 1, 1.0f);
                    
                    auto s1 = std::make_shared<Spring>(p, closestTriangle->p0, stiffness);
                    auto s2 = std::make_shared<Spring>(p, closestTriangle->p1, stiffness);
                    auto s3 = std::make_shared<Spring>(p, closestTriangle->p2, stiffness);
                    if (!closestTriangle->p0->HasFlag(ParticleFlags::PARTICLE_CENTER)) m_springs.push_back(s1);
                    if (!closestTriangle->p1->HasFlag(ParticleFlags::PARTICLE_CENTER)) m_springs.push_back(s2);
                    if (!closestTriangle->p2->HasFlag(ParticleFlags::PARTICLE_CENTER)) m_springs.push_back(s3);
                }
            }
        }

        for (auto& p : particlesNearToTorso) 
        {
            QVector3D pointOnMesh = p->GetPosition();
            GetPointOntoMesh(pointOnMesh);
            if (pointOnMesh.isNull()) continue;
            auto torsoParticle = std::make_shared<Particle>(pointOnMesh, p->GetRadius(), p->GetMass(), false);
            auto s = std::make_shared<Spring>(p, torsoParticle, stiffness);
            m_springs.push_back(s);
        }
            
    }

    doneCurrent();
}

void OpenGLWidget::Add2ndLayer(
    std::vector<std::shared_ptr<Particle>>& particles,
    std::vector<std::shared_ptr<Spring>>& springs,
    std::vector<std::shared_ptr<TriangleCollider>>& triangleColliders,
    const QVector3D& center, const float thickness, const float stiffnessInterLayer, const float stiffnessAreola)
{
    makeCurrent();
    std::unordered_map<TriangleCollider*, std::shared_ptr<Particle>> barycenterMap;
    std::unordered_map<Edge, std::vector<TriangleCollider*>, EdgeHash> edgeToTriangles;
    
    for (const auto& triangle : triangleColliders)
    {   
        if (!triangle->p0 || !triangle->p1 || !triangle->p2) continue;

        auto p0 = triangle->p0;
        auto p1 = triangle->p1;
        auto p2 = triangle->p2;

        QVector3D baryCenter = (p0->GetPosition() + p1->GetPosition() + p2->GetPosition()) / 3.0f;
        baryCenter = baryCenter + (baryCenter - center).normalized() * thickness;

        auto newParticle = std::make_shared<Particle>(baryCenter, p0->GetRadius() * 100.0f, p0->GetMass(), (p0->IsDynamic() && p1->IsDynamic() && p2->IsDynamic()));

        particles.push_back(newParticle);
        barycenterMap[triangle.get()] = newParticle;

        springs.push_back(std::make_shared<Spring>(p0, newParticle, p0->HasFlag(ParticleFlags::PARTICLE_BORDER) ? stiffnessAreola : stiffnessInterLayer));
        springs.push_back(std::make_shared<Spring>(p1, newParticle, p1->HasFlag(ParticleFlags::PARTICLE_BORDER) ? stiffnessAreola : stiffnessInterLayer));
        springs.push_back(std::make_shared<Spring>(p2, newParticle, p2->HasFlag(ParticleFlags::PARTICLE_BORDER) ? stiffnessAreola : stiffnessInterLayer));

        // Create edges and map them to triangles
        edgeToTriangles[makeEdge(p0, p1)].push_back(triangle.get());
        edgeToTriangles[makeEdge(p1, p2)].push_back(triangle.get());
        edgeToTriangles[makeEdge(p2, p0)].push_back(triangle.get());
    }

    for (const auto& [edge, tris] : edgeToTriangles)
    {
        if (tris.size() == 2) {
            auto p1 = barycenterMap[tris[0]];
            auto p2 = barycenterMap[tris[1]];
            if (!p1 || !p2) continue;

            float stiffness = stiffnessInterLayer;

            for (auto& s : springs) {
                if ((s->GetP1() == edge.first && s->GetP2() == edge.second) ||
                    (s->GetP1() == edge.second && s->GetP2() == edge.first)) {
                    stiffness = s->GetStiffness();
                    break;
                }
            }

            springs.push_back(std::make_shared<Spring>(p1, p2, stiffness));
        }
    }



    doneCurrent();
}


void OpenGLWidget::CurveToParticlesSprings() 
{
    if (!m_isCurve || !m_torsoModel) return;

    makeCurrent();

    // Clear previous model
    m_particles.clear();
    m_springs.clear();
    m_triangleColliders.clear();
    m_fillTriangleColliders.clear();
    m_posToAddParticle.clear();

    // Default position
    for (size_t i = 0; i < m_curvePoints.size(); ++i) m_curve.SetControlPoint(i, m_curvePoints[i]);
    
    { // 1st breast  
        m_profilePoints = m_curve.Sample(m_numSamples);
    
        // Remove the last points to avoid duplicates
        m_profilePoints.pop_back();
    
        for (size_t i = 0; i < m_profilePoints.size(); ++i) GetPointOntoMesh(m_profilePoints[i]);

        BuildBreast(m_profilePoints, m_stiffnessBySegment);
    }

    { // 2nd breast (mirrored)
        std::vector<QVector3D> mirroredProfile = m_curve.Sample(m_numSamples);
        for (auto& point : mirroredProfile)
        {
            point.setX(-point.x());
            point.setZ(point.z() + 0.2f); // Add a small offset to avoid z-fighting with the ground
        }

        for (size_t i = 0; i < mirroredProfile.size(); ++i) GetPointOntoMesh(mirroredProfile[i]);
        // std::reverse(mirroredProfile.begin(), mirroredProfile.end()); // Reverse the order of points to match the mirrored profile
        
        // Inverse m_stiffnessBySegment order for the mirrored profile
        std::unordered_map<int, float> mirroredStiffnessBySegment;
        for (const auto& segment : m_stiffnessBySegment) {
            int mirroredIndex = m_nSegments - segment.first - 1;
            mirroredStiffnessBySegment[mirroredIndex] = segment.second;
        }

        BuildBreast(mirroredProfile, mirroredStiffnessBySegment, true);
    }
}



void OpenGLWidget::UpdateCurveForm(int i1, int i2, float value)
{
    QVector3D A = m_curvePointsSliders[i1];
    QVector3D B = m_curvePointsSliders[i2];

    QVector3D center = (A + B) * 0.5f;

    QVector3D dir = (B - A).normalized();
    float halfDist = (B - A).length() * 0.5f;

    float scaledHalfDist = halfDist * (1.0f + value);

    A = center - dir * scaledHalfDist;
    B = center + dir * scaledHalfDist;

    QVector3D tempA = A, tempB = B;
    bool isValid = GetPointOntoMesh(tempA) && GetPointOntoMesh(tempB);

    if (isValid)
    {
        m_curve.SetControlPoint(i1, A);
        m_curve.SetControlPoint(i2, B);

        m_lastValidDeformation = {i1, i2, value};
    } 
    else emit setDeformationSlider(i1, i2, static_cast<int>((std::get<2>(m_lastValidDeformation) + 1.0f) * 50.f));

    m_curvePoints = m_curve.GetControlPoints();
}

void OpenGLWidget::UpdateCurveHeightWidth()
{
    bool isValid = true;
    std::vector<QVector3D> newPoints(m_initialCurvePoints.size());
    for (size_t i = 0; i < m_initialCurvePoints.size(); ++i) 
    {
        QVector3D p = m_initialCurvePoints[i];
        p -= m_curveCenter;

        p.setX(p.x() * m_curveSize/* m_widthScale */);
        p.setY(p.y() * m_curveSize/* m_heightScale */);

        p += m_curveCenter;
        newPoints[i] = p;

        if (!GetPointOntoMesh(p)) 
        {
            isValid = false;
            emit setSizeSlider(static_cast<int>(m_lastValidCurveSize * 50));
            break;
        }

    }

    if (isValid) m_lastValidCurveSize = m_curveSize;

    for (size_t i = 0; i < m_initialCurvePoints.size(); ++i) m_curve.SetControlPoint(i, isValid ? newPoints[i] : m_curvePoints[i]);

    m_curvePoints = m_curve.GetControlPoints();
    m_curvePointsSliders = m_curvePoints;

    for (auto& d : m_curveDeformation) UpdateCurveForm(std::get<0>(d), std::get<1>(d), std::get<2>(d));
}

void OpenGLWidget::addParticle()
{
    makeCurrent();
    bool isRunning = IsRunning();
    Stop();
    for (auto& pos : m_posToAddParticle) {
        auto p = std::make_shared<Particle>(pos + QVector3D(0, 0, 0.24), m_particleRadiusVolume, 1);
        m_physicsSystem->AddRigidbody(p);
        m_physicsSystem->AddConstraint(p);
    }
    if (isRunning) Play();
    doneCurrent();
}

void OpenGLWidget::gravityChanged()
{
    bool isRunning = IsRunning();
    Stop();
    m_physicsSystem->ChangeGravity(m_camera->GetDownVector() * GRAVITY.length());
    if (isRunning) Play(); 
}


void OpenGLWidget::InitVoxelModel()
{
    m_voxel = VoxelGrid(QVector3D(0, 0, 0), QVector3D(5, 5, 5), 0.6f);
    // m_voxel.Generate();
}

void OpenGLWidget::VoxelToParticlesSprings()
{
    if (!m_isVoxelModel) return;

    makeCurrent();

    // Clear previous model
    m_particles.clear();
    m_springs.clear();
    m_triangleColliders.clear();

    m_voxel.SetCross(m_crossSpringModel);

    // Convert the voxel grid into particles and springs
    m_voxel.Generate();
    
    for (auto& p : m_voxel.GetParticles()) m_particles.push_back(p);
    for (auto& s : m_voxel.GetSprings()) m_springs.push_back(s);
    // for (auto& t : m_voxel.triangleColliders) m_physicsSystem->AddTriangleCollider(t);

    doneCurrent();
}

void OpenGLWidget::SetViewMode(ViewMode mode)
{
    switch (mode) 
    {
        case ViewMode::View1:
            m_camera->SetPosition(QVector3D(0, 0, m_camera->GetDistance()));
            m_camera->SetTarget(QVector3D(0, 0, 0));
            break;
        case ViewMode::View2:
            m_camera->SetPosition(QVector3D(m_camera->GetDistance(), 0, 0));
            m_camera->SetTarget(QVector3D(0, 0, 0));
            break;
        case ViewMode::View3:
            m_camera->SetPosition(QVector3D(-m_camera->GetDistance(), 0, 0));
            m_camera->SetTarget(QVector3D(0, 0, 0));
            break;
        case ViewMode::View4:
            m_camera->SetPosition(QVector3D(0, m_camera->GetDistance(), 0));
            m_camera->SetTarget(QVector3D(0, 0, 0));
            break;
        case ViewMode::View5:
            m_camera->SetPosition(QVector3D(0, -m_camera->GetDistance(), 0));
            m_camera->SetTarget(QVector3D(0, 0, 0));
            break;
        case ViewMode::View6:
            m_camera->SetPosition(QVector3D(0, m_camera->GetDistance(), 0));
            m_camera->SetTarget(QVector3D(0, 0, 0));
            m_camera->Rotate(QVector3D(0, -90, 0));
            break;
        
        default:
            qWarning() << "Unknown view mode";
            return;
    }
    
    update();

}

void OpenGLWidget::InitStiffnessValues()
{
    m_stiffness.clear();

    // Create a vector of pairs to hold stiffness values and their names
    std::vector<std::pair<float*, std::string>> stiffnessVars;

    if (m_isCurve)
    {
        // Add stiffness variables to the vector
        stiffnessVars.emplace_back(&m_stiffnessAreola, "areola");
        if (m_isAttached) stiffnessVars.emplace_back(&m_stiffnessAttached, "attached");
        if (m_haveThickness) stiffnessVars.emplace_back(&m_stiffnessInterLayer, "inter_layer");

        // Add stiffness by segment variables
        for (int i = 0; i < m_nSegments; ++i) stiffnessVars.emplace_back(&m_stiffnessBySegment[i], "segment_" + std::to_string(i));
    }

    // Sort the stiffness variables by their values
    std::sort(stiffnessVars.begin(), stiffnessVars.end(),
        [](const auto& a, const auto& b) {
            return *(a.first) < *(b.first);
    });

    // Set the stiffness variables in the m_stiffness map
    for (size_t i = 0; i < stiffnessVars.size(); ++i)  m_stiffness[static_cast<int>(i)] = stiffnessVars[i].first;
}

void OpenGLWidget::SetModelMode(ModelMode mode)
{
    m_mode = mode;
    
    switch (mode)
    {
        case ModelMode::ModelModeMassSpring:
            {
                m_model->Hide();
                for (auto& p : m_particles) p->Show();
                for (auto& s : m_springs) s->Show();
                break;
            }
        case ModelMode::ModelModeSurface:
            {
                m_model->Show();
                for (auto& p : m_particles) p->Hide();
                for (auto& s : m_springs) s->Hide();
                break;
            }
        case ModelMode::ModelModeMassSpringAndSurface:
            {
                m_model->Show();
                for (auto& p : m_particles) p->Show();
                for (auto& s : m_springs) s->Show();
                break;
            }
    }
    
}