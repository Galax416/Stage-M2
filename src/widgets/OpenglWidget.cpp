#include "OpenglWidget.h"
#include "Camera.h"
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

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), m_program(0)
{
    setFocusPolicy(Qt::StrongFocus);  // Permit to receive key events
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();
    // if (m_worker) m_worker->Stop();
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
        m_camera->SetAspect(static_cast<float>(width) / static_cast<float>(height));
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
    m_camera = std::make_shared<Camera>(QVector3D(0.0f, 0.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f));
    
    // Initialize the physics system
    m_physicsSystem = std::make_shared<PhysicsSystem>();
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

    InitScene();
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);

    glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), m_backgroundColor.alphaF());

    glUseProgram(m_program->programId());

    m_program->bind();
    m_program->setUniformValue("projection", m_camera->GetProjectionMatrix());
    m_program->setUniformValue("view", m_camera->GetViewMatrix());

    m_program->setUniformValue("ligth_position", m_camera->GetPosition());
    m_program->setUniformValue("viewPos", m_camera->GetPosition());

    if (m_isWireMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    m_physicsSystem->Render(m_program.get()); // Render the physics system

    m_program->release();
    
    // if (IsRunning()) m_physicsSystem->Update(m_deltaTime); 
    // if (m_bvhTorsoColliders) RenderBVH(m_program.get(), m_bvhTorsoColliders.get());

    update();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    // Camera rotation
    m_camera->mousePressEvent(event);
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // Camera rotation
    m_camera->mouseReleaseEvent(event);
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
    // qDebug() << event->key();
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
        if (IsPaused()) {
            m_renderBVH = !m_renderBVH;
            emit renderBVHChanged(m_renderBVH);
        }
        break;
    case Qt::Key_E:
        {
            makeCurrent();
            auto p = std::make_shared<Particle>(QVector3D(0, 0, 0.2), 6, 5);
            m_physicsSystem->AddRigidbody(p);
            m_physicsSystem->AddConstraint(p);
            doneCurrent();
        }
        break;
    case Qt::Key_R:
        // if (m_curve.IsClosed()) m_curve.SetClosed(false);
        // else m_curve.SetClosed(true);
        Stop();
        Reset();
        break;
    case Qt::Key_C:
        {

        }
    case 16777236: // Right Arrow
        {
            breastDirection += QVector3D(0.1, 0, 0);
            ChangeControlPointPosistion(QVector3D(0.1, 0, 0));
        }
        break;
    case 16777234: // Left Arrow
        {
            breastDirection += QVector3D(-0.1, 0, 0);
            ChangeControlPointPosistion(QVector3D(-0.1, 0, 0));
        }
        break;
    case 16777237: // Down Arrow
        {
            breastDirection += QVector3D(0, -0.1, 0);
            ChangeControlPointPosistion(QVector3D(0, -0.1, 0));
        }
        break;
    case 16777235: // Up Arrow
        {
            breastDirection += QVector3D(0, 0.1, 0);
            ChangeControlPointPosistion(QVector3D(0, 0.1, 0));
        }
        break;
    case 43: // '+'
        {
            makeCurrent();
            // m_press->SetPosition(m_press->GetPosition() + QVector3D(0, -2, 0) * m_deltaTime);
            m_press->SetMass(m_press->GetMass() + 1.0f);
            qDebug() << m_press->GetMass();
            m_press->SynsCollisionVolumes();
            doneCurrent();
        }
        break;
    case 45: // '-'
        {
            makeCurrent();
            // m_press->SetPosition(m_press->GetPosition() + QVector3D(0, 2, 0) * m_deltaTime);
            m_press->SetMass(m_press->GetMass() - 1.0f);
            qDebug() << m_press->GetMass();
            m_press->SynsCollisionVolumes();
            doneCurrent();
        }
        break;
    default:
        break;
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    
}

void OpenGLWidget::InitScene()
{
    // Initialize the scene here
    makeCurrent();    

    // auto ground = std::make_shared<Plane>(QVector3D(0, -2, 0), QVector3D(0, 1, 0), QColor(150, 150, 150));
    // ground->SetStatic();
    // ground->SetColor(m_backgroundColor);
    // m_physicsSystem->AddRigidbody(ground);
    // m_physicsSystem->AddConstraint(ground);
    
    // CreateVoxelGrid(QVector3D(0, 0, 0), QVector3D(2, 2, 2), 10, 10, 10, m_particles, m_springs);
    
    // Test particle
    // auto bottom = std::make_shared<Box>(QVector3D(0, -1, 0), QVector3D(10, 0.2, 10), QColor(150, 150, 150));
    // bottom->SetMovable(false);
    // m_physicsSystem->AddRigidbody(bottom);
    // m_physicsSystem->AddConstraint(bottom);

    // auto front = std::make_shared<Box>(QVector3D(0, 0, -1), QVector3D(1, 1, 0.1), QColor(100, 100, 100));
    // front->SetMovable(false);
    // m_physicsSystem->AddRigidbody(front);
    // m_physicsSystem->AddConstraint(front);

    // auto back = std::make_shared<Box>(QVector3D(0, 0, 1), QVector3D(1, 1, 0.1), QColor(100, 100, 100));
    // back->SetMovable(false);
    // m_physicsSystem->AddRigidbody(back);
    // m_physicsSystem->AddConstraint(back);

    // auto left = std::make_shared<Box>(QVector3D(-1, 0, 0), QVector3D(0.1, 1, 1), QColor(100, 100, 100));
    // left->SetMovable(false);
    // m_physicsSystem->AddRigidbody(left);
    // m_physicsSystem->AddConstraint(left);

    // auto right = std::make_shared<Box>(QVector3D(1, 0, 0), QVector3D(0.1, 1, 1), QColor(100, 100, 100));
    // right->SetMovable(false);
    // m_physicsSystem->AddRigidbody(right);
    // m_physicsSystem->AddConstraint(right);
    
    
    if (m_isCurve)
    { 
        // Clear previous model
        m_model->mesh->clear();
        m_model->customOBJ->clear();

        // for (auto p : m_curve.GetControlPoints()) m_physicsSystem->AddRigidbody(std::make_shared<Particle>(p, 2, 10, false ,QColor(255, 0, 0)));

        // auto plane = std::make_shared<Plane>(QVector3D(0, 0, 0), QVector3D(0, 0, 1));
        // plane->SetStatic();
        // plane->SetColor(m_backgroundColor);
        // m_physicsSystem->AddRigidbody(plane);
        // m_physicsSystem->AddConstraint(plane);

        m_physicsSystem->AddRigidbody(m_torsoModel);
        for (auto& t : m_torsoModel->triangleColliders) m_physicsSystem->AddTriangleCollider(t);


    } 
    else if (m_isVoxelModel) 
    {
        // Clear previous model
        m_model->mesh->clear();
        m_model->customOBJ->clear();

        auto ground = std::make_shared<Box>(QVector3D(0, -2, 0), QVector3D(10, 0.2, 10), QColor(150, 150, 150));
        ground->SetStatic();
        m_physicsSystem->AddRigidbody(ground);
        m_physicsSystem->AddConstraint(ground);

        m_press = std::make_shared<Box>(QVector3D(0, 2, 0), QVector3D(2, 0.2, 2), QColor(200, 200, 200));
        m_press->SetStatic();
        m_press->SetMass(0.0f);
        m_physicsSystem->AddRigidbody(m_press);
        m_physicsSystem->AddConstraint(m_press);

        emit update3DModelParametersChanged(m_voxel);

    } 
    else 
    {
        auto ground = std::make_shared<Box>(QVector3D(0, -2, 0), QVector3D(10, 0.2, 10), QColor(150, 150, 150));
        ground->SetStatic();
        m_physicsSystem->AddRigidbody(ground);
        m_physicsSystem->AddConstraint(ground);

        if (!m_model->customOBJ->isCustomOBJ) {
            // Convert the model into particles and springs
            ConvertModelToParticleSprings(m_model.get(), m_particles, m_springs, m_triangleColliders, !m_crossSpringModel);
        } else {
            // Load the model from file
            ChargeModelParticleSprings(m_model.get(), m_particles, m_springs, m_triangleColliders, !m_crossSpringModel);
        }
    }

    // qDebug() << "Particles: " << m_particles.size() << " Springs: " << m_springs.size() << " Triangle colliders: " << m_triangleColliders.size();
    
    // // Add particles and springs to the physics system
    for (auto& p : m_particles) { m_physicsSystem->AddRigidbody(p); m_physicsSystem->AddConstraint(p); }
    for (auto& s : m_springs) m_physicsSystem->AddSpring(s);
    for (auto& t : m_triangleColliders) m_physicsSystem->AddTriangleCollider(t);

    m_physicsSystem->Update(0.0f); // Initialize the physics system

    m_physicsSystem->ChangeFriction(m_globalFriction);
    m_physicsSystem->RotateRigidbodies(m_globalRotation);

    m_physicsSystem->SetUpBVH();

    emit updateSpringsStiffnessControlsChanged(m_springs);
    
    doneCurrent();
    update();
    
}

void OpenGLWidget::LoadOBJ(const QString& filename)
{
    m_isCurve = false;
    makeCurrent();
    m_model->LoadModel(filename);
    doneCurrent();
    Reset();
}

void OpenGLWidget::SaveOBJ(const QString& filename)
{
    m_model->customOBJ->SaveOBJ(filename);
    // emit statusBarMessageChanged("Model saved");
}

void OpenGLWidget::InitCurves()
{
    // Model
    makeCurrent();
    m_torsoModel = std::make_shared<Model>("./resources/models/torse.obj");
    m_torsoModel->SetStatic();
    // m_torsoModel->transform.scale = QVector3D(5.0f, 5.0f, 5.0f);
    m_torsoModel->SetColor(QColor(100, 100, 100));
    m_bvhTorsoColliders = BuildBVH(m_torsoModel->triangleColliders);
    doneCurrent();

    // Curve
    m_curve.SetCurveType(CurveType::BSpline);
    m_curve.Clear();

    m_curve.AddControlPoint(QVector3D(0, 1, 10));
    m_curve.AddControlPoint(QVector3D(-0.38, 0.92, 10));
    m_curve.AddControlPoint(QVector3D(-0.70, 0.70, 10));
    m_curve.AddControlPoint(QVector3D(-1, 0, 10));
    m_curve.AddControlPoint(QVector3D(-0.70, -0.70, 10));
    m_curve.AddControlPoint(QVector3D(0, -1, 10));
    m_curve.AddControlPoint(QVector3D(0.70, -0.70, 10));
    m_curve.AddControlPoint(QVector3D(1, 0, 10));
    m_curve.AddControlPoint(QVector3D(0.71, 0.71, 10));
    m_curve.AddControlPoint(QVector3D(0.38, 0.92, 10));

    // m_curve.AddControlPoint(QVector3D(0, 1, 0));
    // m_curve.AddControlPoint(QVector3D(-0.38, 0.92, 0));
    // m_curve.AddControlPoint(QVector3D(-0.70, 0.70, 0));
    // m_curve.AddControlPoint(QVector3D(-1, 0, 0));
    // m_curve.AddControlPoint(QVector3D(-0.70, -0.70, 0));
    // m_curve.AddControlPoint(QVector3D(0, -1, 0));
    // m_curve.AddControlPoint(QVector3D(0.70, -0.70, 0));
    // m_curve.AddControlPoint(QVector3D(1, 0, 0));
    // m_curve.AddControlPoint(QVector3D(0.71, 0.71, 0));
    // m_curve.AddControlPoint(QVector3D(0.38, 0.92, 0));

    m_curvePoints = m_curve.GetControlPoints();
    ChangeControlPointPosistion(QVector3D(1, 0.9, 1));
    UpdateCurveHeightWidth();

    m_curvePoints = m_curve.GetControlPoints();
    m_curvePointsSliders = m_curve.GetControlPoints();

    Reset();
}

void OpenGLWidget::ChangeControlPointPosistion(const QVector3D& direction)
{
    emit breastSlidersChanged();
    
    for (size_t i = 0; i < m_curvePoints.size(); ++i)
    {
        auto& pt = m_curvePoints[i];
        pt.setX(pt.x() + direction.x());
        pt.setY(pt.y() + direction.y());
        pt.setZ(pt.z() + direction.z());
        
        m_curve.SetControlPoint(i, pt);
    }
    
    Reset();
}

QVector3D OpenGLWidget::GetPointOntoMesh(const QVector3D& point)
{
    // Project the point onto the mesh
    RayCastResult result;

    Ray ray(point, QVector3D(0, 0, -1));

    float minDistSq = std::numeric_limits<float>::max();
    QVector3D closestPoint = point;

    std::vector<std::shared_ptr<TriangleCollider>> triangles;
    QueryBVH<TriangleCollider>(AABB(point - QVector3D(0.01,0.01,0.01), point + QVector3D(0.01,0.01,0.01)), m_bvhTorsoColliders.get(), triangles);

    for (const auto& triangle : triangles)
    {
        result = RayIntersectsTriangle(ray, triangle);
        if (result.hit)
        {
            float distSq = (result.point - point).lengthSquared();
            if (distSq < minDistSq)
            {
                minDistSq = distSq;
                closestPoint = result.point;
            }
        }
    }

    return closestPoint;

}

/*
void OpenGLWidget::CurveToParticlesSprings()
{
    if (!m_isCurve || !m_torsoModel) return;

    makeCurrent();
    // Clear previous model
    m_particles.clear();
    m_springs.clear();
    m_triangleColliders.clear();
    m_fillTriangleColliders.clear();

    // Parameters
    float mass = 1.0f;
    int numLayers = m_curveLayers + 2;
    float height = m_curveDepth;
    float layerStep = m_curveDepth / (numLayers - 1);
    float ringRadius = m_curveRingRadius;

    // Model curve
    m_profilePoints = m_curve.Sample(m_numSamples);

    // Remove the last points to avoid duplicates
    m_profilePoints.pop_back();

    size_t numPoints = m_profilePoints.size();
    if (numPoints < 3) return; // Not enough profilePoints to create a curve

    // Place sample points in the torso model
    for (size_t i = 0; i < numPoints; ++i) 
    {
        QVector3D p = GetPointOntoMesh(m_profilePoints[i]);
        m_profilePoints[i] = p;
    }

    // Compute the center of the curve
    QVector3D centerCurve(0, 0, 0);
    QVector3D centerRing(0, 0, 0);
    for (const auto& pt : m_profilePoints) centerCurve += pt;
    centerCurve /= static_cast<float>(numPoints);
    
    centerRing = centerCurve;

    // Compute the normal of the curve
    for (size_t i = 0; i < numPoints; ++i)
    {
        auto& p1 = m_profilePoints[i];
        auto& p2 = m_profilePoints[(i + 1) % numPoints];
        auto& p3 = m_profilePoints[(i + 2) % numPoints];

        QVector3D v1 = p2 - p1;
        QVector3D v2 = p3 - p2;

        m_curveNormal += QVector3D::crossProduct(v1, v2).normalized();
    }
    m_curveNormal.normalize();

    // Add offset to the center
    centerRing.setY(centerRing.y() - 0.2f);
    // Set the center to he height
    // centerRing += m_curveNormal * height;
    
    QVector3D zAxis = m_curveNormal;
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
        QVector3D p = cosAngle * xAxis * ringRadius + sinAngle * yAxis * ringRadius;

        ringPoints.push_back(centerRing + p);
    }

    centerRing.setZ(centerRing.z() + height);

    // Center particle
    auto centerParticle = std::make_shared<Particle>(centerRing, 1, mass);
    centerParticle->SetFlags(PARTICLE_NO_COLLISION_WITH_US);
    m_particles.push_back(centerParticle);

    // Create the inner layer particle
    std::vector<std::vector<std::vector<std::shared_ptr<Particle>>>> innerLayers;
    
    // Create the vector of stiffness for the springs
    std::vector<float> springStiffness(numPoints, 0.0f), springCrossStiffness(numPoints, 0.0f);
    
    for (int innerLayer = 0; innerLayer < 5; ++innerLayer)
    {
        // Create the layers of particles
        std::vector<std::vector<std::shared_ptr<Particle>>> layers;

        // For each innerLayer, shrink m_profilePoints towards centerCurve
        std::vector<QVector3D> layerProfilePoints = m_profilePoints;
        float shrinkFactor = 1.0f - (static_cast<float>(innerLayer) / 5.0f); // 5 is the number of inner layers
        for (size_t i = 0; i < layerProfilePoints.size(); ++i) {
            layerProfilePoints[i] = centerCurve + (layerProfilePoints[i] - centerCurve) * shrinkFactor;
        }

        for (int layer = 0; layer < numLayers; ++layer)
        {
            std::vector<std::shared_ptr<Particle>> layerParticles;

            if (layer == numLayers - 1 && innerLayer > 0) {
                layerParticles = innerLayers[innerLayer - 1].back();
                layers.push_back(layerParticles);
                continue;
            }
    
            float t = static_cast<float>(layer) / static_cast<float>(numLayers - 1);
            float tCurve = 1.0f + std::sin(-(1.0f - t) * M_PI_2); 
            float z = layer * layerStep;
    
            for (size_t i = 0; i < numPoints; ++i) {      
                QVector3D pos = (1.0f - tCurve) * layerProfilePoints[i] + tCurve * ringPoints[i];
    
                pos.setZ(pos.z() + z);
    
                auto p = std::make_shared<Particle>(pos, 1, innerLayer == 0 ? 10 : 30, (layer != 0));
                p->AddFlag(PARTICLE_NO_COLLISION_WITH_US); p->AddFlag(PARTICLE_ATTACHED_TO_TRIANGLE);
                layerParticles.push_back(p);
                m_particles.push_back(p);
    
                // Horizontally connect particles in the same layer
                if (i > 0) 
                {
                    auto& p1 = layerParticles[i - 1];
                    auto& p2 = layerParticles[i];
                    if (layer == 0) springStiffness[i - 1] = GetStiffnessByQuadrant(p1->GetPosition(), p2->GetPosition(), centerCurve);
                    float stiffness = springStiffness[i - 1];
                    auto spring = std::make_shared<Spring>(p1, p2, (layer == numLayers - 1) ? 1000.0f : stiffness);
                    m_springs.push_back(spring);
                }
            }
    
            // Close the curve
            auto& p1 = layerParticles.front();
            auto& p2 = layerParticles.back();
            if (layer == 0) springStiffness[numPoints - 1] = GetStiffnessByQuadrant(p1->GetPosition(), p2->GetPosition(), centerCurve);
            float stiffness = springStiffness[numPoints - 1];
            auto springLoop = std::make_shared<Spring>(p1, p2, (layer == numLayers - 1) ? 1000.0f : stiffness);
            m_springs.push_back(springLoop);
    
            layers.push_back(layerParticles);
        }

        innerLayers.push_back(layers);

        // Connect the center to the last layer
        if (innerLayer == 0) {
            for (auto& p : innerLayers[innerLayer].back()) {
                auto spring = std::make_shared<Spring>(p, centerParticle, 1000.0f); 
                m_springs.push_back(spring);
            }
            auto ringlayers = innerLayers[innerLayer].back();
            for (size_t i = 0; i < ringlayers.size(); ++i) { // Add springs for more rigidity
                auto& p1 = ringlayers[i];
                for (size_t j = 0; j < ringlayers.size(); ++j) {
                    if (i == j) continue;
                    auto& p2 = ringlayers[j];
                    auto spring = std::make_shared<Spring>(p1, p2, 1000.0f);
                    m_springs.push_back(spring);
                }
            }
        }

        
        // Vertical and diagonal connections between layers
        for (int layer = 0; layer < numLayers - 1; ++layer) {

            for (size_t i = 0; i < numPoints; ++i) {

                // edge
                auto& p1 = innerLayers[innerLayer][layer][i];
                auto& p2 = innerLayers[innerLayer][layer + 1][i];
                if (layer == 0) springStiffness[i] = GetStiffnessByQuadrant(p1->GetPosition(), p2->GetPosition(), centerCurve);
                float stiffness = springStiffness[i];
                auto spring = std::make_shared<Spring>(p1, p2, stiffness);
                m_springs.push_back(spring);

                // diagonal
                if (i > 0 && m_crossSpringModel) 
                {
                    auto& p3 = innerLayers[innerLayer][layer][i];
                    auto& p4 = innerLayers[innerLayer][layer + 1][i - 1];
                    if (layer == 0) springCrossStiffness[i - 1] = GetStiffnessByQuadrant(p3->GetPosition(), p4->GetPosition(), centerCurve);
                    float stiffness = springCrossStiffness[i - 1];
                    auto s1 = std::make_shared<Spring>(p3, p4, stiffness);
                    m_springs.push_back(s1);
                    
                    auto& p5 = innerLayers[innerLayer][layer][i - 1];
                    auto& p6 = innerLayers[innerLayer][layer + 1][i];
                    if (layer == 0) springCrossStiffness[i - 1] = GetStiffnessByQuadrant(p5->GetPosition(), p6->GetPosition(), centerCurve);
                    stiffness = springCrossStiffness[i - 1];
                    auto s2 = std::make_shared<Spring>(p5, p6, stiffness);
                    m_springs.push_back(s2);
                }
            }
            // close the curve
            if (m_crossSpringModel)
            {
                auto& p1 = innerLayers[innerLayer][layer].back();
                auto& p2 = innerLayers[innerLayer][layer + 1].front();
                float stiffness = springStiffness[numPoints - 1];
                auto s1 = std::make_shared<Spring>(p1, p2, stiffness);
                m_springs.push_back(s1);

                auto& p3 = innerLayers[innerLayer][layer].front();
                auto& p4 = innerLayers[innerLayer][layer + 1].back();
                stiffness = springStiffness[numPoints - 1];
                auto s2 = std::make_shared<Spring>(p3, p4, stiffness);
                m_springs.push_back(s2);
            }
        }
    }
    // Set the last springCrossStiffness to the last value to avoid out of bounds access
    springCrossStiffness[numPoints - 1] = springCrossStiffness[numPoints - 2];

    // Connect innerLayers between them (inter-layer edge & diagonal springs)
    for (int l = 0; l < static_cast<int>(innerLayers.size()) - 1; ++l)
    {
        auto& current = innerLayers[l];
        auto& next = innerLayers[l + 1];

        for (int layer = 0; layer < numLayers; ++layer) {
            for (size_t i = 0; i < numPoints; ++i) {
                auto& p1 = current[layer][i];
                auto& p2 = next[layer][i];

                float stiffness = springStiffness[i];
                m_springs.push_back(std::make_shared<Spring>(p1, p2, stiffness));

                if (m_crossSpringModel) {
                    // diagonals
                    auto& p1_diag = current[layer][i];
                    auto& p2_diag = next[layer][(i + 1) % numPoints];

                    float diagStiffness = springCrossStiffness[i] * 1.2f;
                    m_springs.push_back(std::make_shared<Spring>(p1_diag, p2_diag, diagStiffness));

                    auto& p3_diag = current[layer][(i + 1) % numPoints];
                    auto& p4_diag = next[layer][i];

                    diagStiffness = springCrossStiffness[i] * 1.2f;
                    m_springs.push_back(std::make_shared<Spring>(p3_diag, p4_diag, diagStiffness));
                }
            }
        }
    }

    doneCurrent();
}

void OpenGLWidget::FillVolumeWithParticle()
{

}
*/

void OpenGLWidget::CurveToParticlesSprings()
{
    if (!m_isCurve || !m_torsoModel) return;

    makeCurrent();
    // Clear previous model
    m_particles.clear();
    m_springs.clear();
    m_triangleColliders.clear();
    m_fillTriangleColliders.clear();

    // Parameters
    float mass = 1.0f;
    int numLayers = m_curveLayers + 2;
    float height = m_curveDepth;
    float layerStep = m_curveDepth / (numLayers - 1);
    float ringRadius = m_curveRingRadius;
    float thickness = 0.03f; 

    // Model curve
    m_profilePoints = m_curve.Sample(m_numSamples);

    // Remove the last points to avoid duplicates
    m_profilePoints.pop_back();

    size_t numPoints = m_profilePoints.size();
    if (numPoints < 3) return; // Not enough profilePoints to create a curve

    // Place sample points in the torso model
    for (size_t i = 0; i < numPoints; ++i) 
    {
        QVector3D p = GetPointOntoMesh(m_profilePoints[i]);
        m_profilePoints[i] = p;
    }

    // Compute the center of the curve
    QVector3D centerCurve(0, 0, 0);
    QVector3D centerRing(0, 0, 0);
    for (const auto& pt : m_profilePoints) centerCurve += pt;
    centerCurve /= static_cast<float>(numPoints);
    
    centerRing = centerCurve;

    // Compute the normal of the curve
    for (size_t i = 0; i < numPoints; ++i)
    {
        auto& p1 = m_profilePoints[i];
        auto& p2 = m_profilePoints[(i + 1) % numPoints];
        auto& p3 = m_profilePoints[(i + 2) % numPoints];

        QVector3D v1 = p2 - p1;
        QVector3D v2 = p3 - p2;

        m_curveNormal += QVector3D::crossProduct(v1, v2).normalized();
    }
    m_curveNormal.normalize();

    // Add offset to the center
    centerRing.setY(centerRing.y() - 0.2f);
    // Set the center to he height
    // centerRing += m_curveNormal * height;
    
    QVector3D zAxis = m_curveNormal;
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
        QVector3D p = cosAngle * xAxis * ringRadius + sinAngle * yAxis * ringRadius;

        ringPoints.push_back(centerRing + p);
    }

    // Add a closed triangle 
    std::vector<std::shared_ptr<TriangleCollider>> closingTriangles;

    for (size_t i = 0; i < numPoints; ++i) {
        const QVector3D& a = m_profilePoints[i];
        const QVector3D& b = m_profilePoints[(i + 1) % numPoints]; // wrap around

        // Triangle (a, b, center)
        auto tri = std::make_shared<TriangleCollider>(a, b, centerCurve);
        closingTriangles.push_back(tri);
    }

    m_fillTriangleColliders.insert(m_fillTriangleColliders.end(), closingTriangles.begin(), closingTriangles.end());

    // Create the layers of particles
    std::vector<std::vector<std::shared_ptr<Particle>>> layers;

    // Create the vector of stiffness for the springs
    std::vector<float> springStiffness(numPoints, 0.0f), 
        springCrossStiffness(numPoints, 0.0f),
        springThicknessStiffness(numPoints, 0.0f);

    for (int layer = 0; layer < numLayers; ++layer)
    {
        std::vector<std::shared_ptr<Particle>> layerParticles;

        float t = static_cast<float>(layer) / static_cast<float>(numLayers - 1);
        float tCurve = 1.0f + std::sin(-(1.0f - t) * M_PI_2); 
        float z = layer * layerStep;

        for (size_t i = 0; i < numPoints; ++i) {
            QVector3D pos = (1.0f - tCurve) * m_profilePoints[i] + tCurve * ringPoints[i];
            // QVector3D pos = (1.0f - tCurve) * ringPoints[i] + tCurve * m_profilePoints[i];
            // pos.setZ(m_profilePoints[i].z() + z); 

            // float length = (m_profilePoints[i] - ringPoints[i]).length();
            // float step = length / (numLayers - 1);
            // float z = layer * step;

            pos.setZ(pos.z() + z);


            auto p = std::make_shared<Particle>(pos, 1, mass, (layer != 0));
            p->AddFlag(PARTICLE_NO_COLLISION_WITH_US); p->AddFlag(PARTICLE_ATTACHED_TO_TRIANGLE);
            layerParticles.push_back(p);
            m_particles.push_back(p);

            // Horizontally connect particles in the same layer
            if (i > 0) 
            {
                auto& p1 = layerParticles[i - 1];
                auto& p2 = layerParticles[i];
                if (layer == 0) springStiffness[i - 1] = GetStiffnessByQuadrant(p1->GetPosition(), p2->GetPosition(), centerCurve);
                float stiffness = springStiffness[i - 1];
                auto spring = std::make_shared<Spring>(p1, p2, (layer == numLayers - 1) ? 1000.0f : stiffness);
                m_springs.push_back(spring);
            }
        }

        // Close the curve
        auto& p1 = layerParticles.front();
        auto& p2 = layerParticles.back();
        if (layer == 0) springStiffness[numPoints - 1] = GetStiffnessByQuadrant(p1->GetPosition(), p2->GetPosition(), centerCurve);
        float stiffness = springStiffness[numPoints - 1];
        auto springLoop = std::make_shared<Spring>(p1, p2, (layer == numLayers - 1) ? 1000.0f : stiffness);
        m_springs.push_back(springLoop);

        layers.push_back(layerParticles);
    }

    // Vertical and diagonal connections between layers
    for (int layer = 0; layer < numLayers - 1; ++layer) {
        for (size_t i = 0; i < numPoints; ++i) {

            // edge
            auto& p1 = layers[layer][i];
            auto& p2 = layers[layer + 1][i];
            if (layer == 0) springStiffness[i] = GetStiffnessByQuadrant(p1->GetPosition(), p2->GetPosition(), centerCurve);
            float stiffness = springStiffness[i];
            auto spring = std::make_shared<Spring>(p1, p2, stiffness);
            m_springs.push_back(spring);

            // diagonal
            if (i > 0 && m_crossSpringModel) 
            {
                auto& p3 = layers[layer][i];
                auto& p4 = layers[layer + 1][i - 1];
                if (layer == 0) springCrossStiffness[i - 1] = GetStiffnessByQuadrant(p3->GetPosition(), p4->GetPosition(), centerCurve);
                float stiffness = springCrossStiffness[i - 1];
                auto s1 = std::make_shared<Spring>(p3, p4, stiffness);
                m_springs.push_back(s1);
                
                auto& p5 = layers[layer][i - 1];
                auto& p6 = layers[layer + 1][i];
                if (layer == 0) springCrossStiffness[i - 1] = GetStiffnessByQuadrant(p5->GetPosition(), p6->GetPosition(), centerCurve);
                stiffness = springCrossStiffness[i - 1];
                auto s2 = std::make_shared<Spring>(p5, p6, stiffness);
                m_springs.push_back(s2);
            }
        }
        // close the curve
        if (m_crossSpringModel)
        {
            auto& p1 = layers[layer].back();
            auto& p2 = layers[layer + 1].front();
            float stiffness = springStiffness[numPoints - 1];
            auto s1 = std::make_shared<Spring>(p1, p2, stiffness);
            m_springs.push_back(s1);

            auto& p3 = layers[layer].front();
            auto& p4 = layers[layer + 1].back();
            stiffness = springStiffness[numPoints - 1];
            auto s2 = std::make_shared<Spring>(p3, p4, stiffness);
            m_springs.push_back(s2);
        }
        
    }

    centerRing.setZ(centerRing.z() + height);

    // Center particle
    auto centerParticle = std::make_shared<Particle>(centerRing, 1, mass);
    centerParticle->SetFlags(PARTICLE_NO_COLLISION_WITH_US);
    m_particles.push_back(centerParticle);

    // Connect the center to the first layer
    for (auto& p : layers.back()) {
        auto spring = std::make_shared<Spring>(p, centerParticle, 1000.0f); 
        m_springs.push_back(spring);
    }
    auto ringlayers = layers.back();
    for (size_t i = 0; i < ringlayers.size(); ++i) { // Add springs for more rigidity
        auto& p1 = ringlayers[i];
        for (size_t j = 0; j < ringlayers.size(); ++j) {
            if (i == j) continue;
            auto& p2 = ringlayers[j];
            auto spring = std::make_shared<Spring>(p1, p2, 1000.0f);
            m_springs.push_back(spring);
        }
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
            a->AddFlag(PARTICLE_ATTACHED_TO_TRIANGLE);
            auto b = next[i];
            b->AddFlag(PARTICLE_ATTACHED_TO_TRIANGLE);
            auto c = next[next_i];
            c->AddFlag(PARTICLE_ATTACHED_TO_TRIANGLE);

            // Second triangle of the quad
            auto d = current[next_i];

            m_triangleColliders.push_back(std::make_shared<TriangleCollider>(a, b, c));
            m_triangleColliders.push_back(std::make_shared<TriangleCollider>(a, c, d));
        }
    }

    for (size_t i = 0; i < ringlayers.size(); ++i)
    {
        size_t next_i = (i + 1) % ringlayers.size();

        auto a = ringlayers[i];
        a->AddFlag(PARTICLE_ATTACHED_TO_TRIANGLE);
        auto b = ringlayers[next_i];
        b->AddFlag(PARTICLE_ATTACHED_TO_TRIANGLE);
        auto c = centerParticle;
        c->AddFlag(PARTICLE_ATTACHED_TO_TRIANGLE);

        m_triangleColliders.push_back(std::make_shared<TriangleCollider>(a, b, c));
    }

    m_fillTriangleColliders.insert(m_fillTriangleColliders.end(), m_triangleColliders.begin(), m_triangleColliders.end());

    // Thickness
    if (m_haveThickness) {
        std::vector<std::vector<std::shared_ptr<Particle>>> layersThickness;
        for (int layer = 0; layer < numLayers; ++layer)
        {
            std::vector<std::shared_ptr<Particle>> layerParticlesThickness;
            for (size_t i = 0; i < numPoints; ++i)
            {
                auto p = layers[layer][i];
                QVector3D normal = (p->GetPosition() - QVector3D(centerRing.x(), centerRing.y(), centerRing.z() * 0.5f)).normalized();
                QVector3D offsetPos = p->GetPosition() + normal * thickness;
                
                auto pt = std::make_shared<Particle>(offsetPos, 1, p->GetMass(), p->IsDynamic());
                pt->SetFlags(PARTICLE_NO_COLLISION_WITH_US); p->AddFlag(PARTICLE_ATTACHED_TO_TRIANGLE);
                layerParticlesThickness.push_back(pt);
                m_particles.push_back(pt);

                // Connect the new particle to the original one
                if (layer == 0) springStiffness[i] = GetStiffnessByQuadrant(p->GetPosition(), pt->GetPosition(), centerCurve);
                float stiffness = springStiffness[i];
                auto spring = std::make_shared<Spring>(p, pt, stiffness);
                m_springs.push_back(spring);

                if (i > 0) {
                    auto& p1 = layerParticlesThickness[i - 1];
                    auto& p2 = layerParticlesThickness[i];
                    if (layer == 0) springThicknessStiffness[i - 1] = GetStiffnessByQuadrant(p1->GetPosition(), p2->GetPosition(), centerCurve);
                    float stiffness = springThicknessStiffness[i - 1];
                    auto spring = std::make_shared<Spring>(p1, p2, stiffness);
                    m_springs.push_back(spring);
                }
            }

            // Close the curve
            auto& p1 = layerParticlesThickness.front();
            auto& p2 = layerParticlesThickness.back();
            if (layer == 0) springStiffness[numPoints - 1] = GetStiffnessByQuadrant(p1->GetPosition(), p2->GetPosition(), centerCurve);
            float stiffness = springStiffness[numPoints - 1];
            auto springLoop = std::make_shared<Spring>(p1, p2, stiffness);
            m_springs.push_back(springLoop);

            layersThickness.push_back(layerParticlesThickness);
        }

        // Vertical and diagonal connections between layers
        for (int layer = 0; layer < numLayers - 1; ++layer) {
            for (size_t i = 0; i < numPoints; ++i) {
                // edge
                auto& p1 = layersThickness[layer][i];
                auto& p2 = layersThickness[layer + 1][i];
                float stiffness = springStiffness[i];
                auto spring = std::make_shared<Spring>(p1, p2, (layer == numLayers - 1) ? 1000.0f : stiffness);
                m_springs.push_back(spring);

                // diagonal
                if (i > 0 && m_crossSpringModel) 
                {
                    auto& p3 = layersThickness[layer][i];
                    auto& p4 = layersThickness[layer + 1][i - 1];
                    float stiffness = springCrossStiffness[i - 1];
                    auto s1 = std::make_shared<Spring>(p3, p4, stiffness);
                    m_springs.push_back(s1);
                    
                    auto& p5 = layersThickness[layer][i - 1];
                    auto& p6 = layersThickness[layer + 1][i];
                    stiffness = springCrossStiffness[i - 1];
                    auto s2 = std::make_shared<Spring>(p5, p6, stiffness);
                    m_springs.push_back(s2);
                }
            }

            if (m_crossSpringModel)
            {
                // Close the curve
                auto& p1 = layersThickness[layer].back();
                auto& p2 = layersThickness[layer + 1].front();
                float stiffness = m_springs[numPoints - 1]->GetStiffness();
                auto s1 = std::make_shared<Spring>(p1, p2, stiffness);
                m_springs.push_back(s1);

                auto& p3 = layersThickness[layer].front();
                auto& p4 = layersThickness[layer + 1].back();
                stiffness = m_springs[numPoints - 1]->GetStiffness();
                auto s2 = std::make_shared<Spring>(p3, p4, stiffness);
                m_springs.push_back(s2);

                // Add springs between the thickness particles
                for (size_t i = 0; i < numPoints; ++i) {
                    size_t next_i = (i + 1) % numPoints;

                    auto& a = layers[layer][i];
                    auto& b = layers[layer][next_i];
                    auto& a_thick = layersThickness[layer][i];
                    auto& b_thick = layersThickness[layer][next_i];

                    float stiffness = m_springs[i]->GetStiffness();
                    auto s1 = std::make_shared<Spring>(a, b_thick, stiffness);
                    m_springs.push_back(s1);

                    stiffness = m_springs[i]->GetStiffness();
                    auto s2 = std::make_shared<Spring>(b, a_thick, stiffness);
                    m_springs.push_back(s2);

                    auto& c = layers[layer + 1][i];

                    stiffness = springStiffness[i];
                    auto s3 = std::make_shared<Spring>(c, a_thick, stiffness);
                    m_springs.push_back(s3);

                    auto& c_thick = layersThickness[layer + 1][i];
                    stiffness = springStiffness[i];
                    auto s4 = std::make_shared<Spring>(a, c_thick, stiffness);
                    m_springs.push_back(s4);
                    
                }
            }
        }

        // Center particle
        auto centerParticleThickness = std::make_shared<Particle>(centerRing + QVector3D(0, 0, thickness), 1, mass);
        centerParticleThickness->SetFlags(PARTICLE_NO_COLLISION_WITH_US);
        m_particles.push_back(centerParticleThickness);

        // Connect the center to the first layer
        for (auto& p : layersThickness.back()) {
            auto spring = std::make_shared<Spring>(p, centerParticleThickness, 1000.0f); 
            m_springs.push_back(spring);
        }
        auto ringlayers = layersThickness.back();
        for (size_t i = 0; i < ringlayers.size(); ++i) { // Add springs for more rigidity
            auto& p1 = ringlayers[i];
            for (size_t j = 0; j < ringlayers.size(); ++j) {
                if (i == j) continue;
                auto& p2 = ringlayers[j];
                auto spring = std::make_shared<Spring>(p1, p2, 1000.0f);
                m_springs.push_back(spring);
            }
        }
    }

    doneCurrent();

    FillVolumeWithParticle();
}

void OpenGLWidget::FillVolumeWithParticle()
{
    if (!m_isCurve) return;

    makeCurrent();

    float particleMass = 10.0f;
    float particleRadius = 6.0f;
    float spacing = 0.12f;

    // float particleMass = 5.0f;
    // float particleRadius = 17.0f;
    // float spacing = 0.3f;

    std::unique_ptr<BVHNode<TriangleCollider>> bvh = BuildBVH(m_fillTriangleColliders);

    QVector3D minBound = m_profilePoints[0];
    QVector3D maxBound = m_profilePoints[0];

    // Compute the bounding box of the curve
    for (const auto& pt : m_profilePoints) {
        minBound.setX(std::min(minBound.x(), pt.x()));
        minBound.setY(std::min(minBound.y(), pt.y()));

        maxBound.setX(std::max(maxBound.x(), pt.x()));
        maxBound.setY(std::max(maxBound.y(), pt.y()));
    }

    minBound.setZ(minBound.z() + particleRadius * 0.01f);
    maxBound.setZ(maxBound.z() + (m_curveDepth * m_curveNormal).z() + particleRadius * 0.01f);

    int countX = static_cast<int>((maxBound.x() - minBound.x()) / spacing) + 1;
    int countY = static_cast<int>((maxBound.y() - minBound.y()) / spacing) + 1;
    int countZ = static_cast<int>((maxBound.z() - minBound.z()) / spacing) + 1;

    auto getIndex = [&](int x, int y, int z) {
        return x + y * countX + z * countX * countY;
    };

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

                if (IsParticleInsideMesh(p, trianglesClosest)){
                    temp[getIndex(i, j, k)] = p;
                }
            }
        }
    }

    std::vector<std::shared_ptr<Particle>> particlesToAdd;

    for (int i = 0; i < countX; ++i) {
        for (int j = 0; j < countY; ++j) {
            for (int k = 0; k < countZ; ++k) {
                auto p = temp[getIndex(i, j, k)];
                if (!p) continue;

                // X+1
                if (i + 1 < countX) {
                    auto neighbor = temp[getIndex(i + 1, j, k)];
                    if (neighbor) m_springs.push_back(std::make_shared<Spring>(p, neighbor, 200));
                }
                // Y+1
                if (j + 1 < countY) {
                    auto neighbor = temp[getIndex(i, j + 1, k)];
                    if (neighbor) m_springs.push_back(std::make_shared<Spring>(p, neighbor, 200));
                }
                // Z+1
                if (k + 1 < countZ) {
                    auto neighbor = temp[getIndex(i, j, k + 1)];
                    if (neighbor) m_springs.push_back(std::make_shared<Spring>(p, neighbor, 200));
                }

                particlesToAdd.push_back(p);
                m_particles.push_back(p);
            }
        }
    }

    float threshold = spacing * 1.5f;

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
            if (!closestTriangle->p0) continue;
            auto spring = std::make_shared<Spring>(p, closestTriangle->p0, 200.0f);
            m_springs.push_back(spring);
        }
    }


    doneCurrent();
}

void OpenGLWidget::setDeformation(int p1, int p2, float value)
{
    if (!m_isCurve) return;

    p1--;
    p2--;

    QVector3D A = m_curvePointsSliders[p1];
    QVector3D B = m_curvePointsSliders[p2];

    QVector3D center = (A + B) * 0.5f;

    QVector3D dir = (B - A).normalized();
    float halfDist = (B - A).length() * 0.5f;

    float scaledHalfDist = halfDist * (1.0f + value);

    A = center - dir * scaledHalfDist;
    B = center + dir * scaledHalfDist;

    if (p1 != 0) m_curve.SetControlPoint(p1, A);
    if (p2 != 0) m_curve.SetControlPoint(p2, B);

    Reset();
}

void OpenGLWidget::setCurveWidth(float value)
{
    if (!m_isCurve) return;
    m_widthScale = (value + 1.0f);
    UpdateCurveHeightWidth();    
}

void OpenGLWidget::setCurveHeight(float value)
{
    if (!m_isCurve) return;
    m_heightScale = (value + 1.0f);
    UpdateCurveHeightWidth();
}

void OpenGLWidget::UpdateCurveHeightWidth()
{
    for (size_t i = 0; i < m_curvePoints.size(); ++i) 
    {
        auto p = m_curvePoints[i];
        p.setX(p.x() * m_widthScale);
        p.setY(p.y() * m_heightScale);
        m_curve.SetControlPoint(i, p);
    }
    Reset();

    m_curvePointsSliders = m_curve.GetControlPoints();
}

void OpenGLWidget::setCurveDepth(float value)
{
    if (!m_isCurve) return;
    m_curveDepth = (value + 1.0f);
    Reset();
}

void OpenGLWidget::setCurveRing(float radius)
{
    if (!m_isCurve) return;
    m_curveRingRadius = (radius + 1.0f) * 0.5f * 0.2f;
    Reset();
}

void OpenGLWidget::InitVoxelModel()
{
    m_voxel = VoxelGrid(QVector3D(0, 0, 0), QVector3D(11, 11, 11), 0.2f);
    // m_voxel.Generate();

    Reset();
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
