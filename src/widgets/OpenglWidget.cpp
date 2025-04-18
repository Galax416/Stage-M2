#include "OpenglWidget.h"
#include "Camera.h"
#include "Geometry3D.h"
#include "ModelPhysicsConverter.h"
#include "Utils.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), m_program(0)
{
    setFocusPolicy(Qt::StrongFocus);  // Permit to receive key events
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();
    // m_vao->destroy();
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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


    // m_vao = new QOpenGLVertexArrayObject();
    // m_vao->create();

    // Initializa shaders programs
    // m_program = new QOpenGLShaderProgram();
    m_program2D = std::make_shared<QOpenGLShaderProgram>();
    m_program3D = std::make_shared<QOpenGLShaderProgram>();

    // 2D shaders
    InitShaders(m_program2D.get(), ":/shaders/2D.vert", "", ":/shaders/2D.frag");
    // 3D shaders
    InitShaders(m_program3D.get(), ":/shaders/3D.vert", "" , ":/shaders/3D.frag");

    if ( m_is2DMode ) m_program = m_program2D.get();
    else m_program = m_program3D.get();

    // Initialize the camera
    m_camera = new Camera(QVector3D(0.0f, 0.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f));
    
    // Initialize the physics system
    // m_physicsSystem = PhysicsSystem();

    // Initialize the model
    m_model = new Model();

    connect(this, &OpenGLWidget::renderBVHChanged, &m_physicsSystem, &PhysicsSystem::renderBVH);
    
    InitScene();

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
    
    m_physicsSystem.Render(m_program);

    m_program->release();
    
    if (!m_isPaused) m_physicsSystem.Update(m_deltaTime);

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
    qDebug() << event->key();
    switch (event->key())
    {
    case Qt::Key_Space:
        if (m_isPaused) Play();
        else Stop();
        break;
    case Qt::Key_W:
        m_isWireMode = !m_isWireMode;
        break;
    case Qt::Key_B:
        emit renderBVHChanged();
        break;
    case Qt::Key_E:
        makeCurrent();
        m_particles.push_back(std::make_shared<Particle>(QVector3D(0, 0, 0.2), 5, 10, true, QColor(255, 0, 255)));
        m_physicsSystem.AddRigidbody(m_particles[m_particles.size() - 1]);
        m_physicsSystem.AddConstraint(m_particles[m_particles.size() - 1]);
        doneCurrent();
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

    Stop();

    auto ground = std::make_shared<Plane>(QVector3D(0, -2, 0), QVector3D(0, 1, 0), QColor(150, 150, 150));
    ground->SetMovable(false);
    m_physicsSystem.AddRigidbody(ground);
    m_physicsSystem.AddConstraint(ground);

    // auto plane = std::make_shared<Plane>(QVector3D(0, 0, 0), QVector3D(0, 0, 1), QColor(50, 50, 50));
    // plane->SetMovable(false);
    // m_physicsSystem.AddRigidbody(plane);
    // m_physicsSystem.AddConstraint(plane);

    if (!m_isCurve)
    { 
        if (!m_model->customOBJ->isCustomOBJ) {
            // Convert the model into particles and springs
            ConvertModelToParticleSprings(m_model, m_particles, m_springs, m_triangleColliders, !m_crossSpringModel);
        } else {
            // Load the model from file
            ChargeModelParticleSprings(m_model, m_particles, m_springs, m_triangleColliders, !m_crossSpringModel);
        }
    } else {
        // for (auto p : m_curve.GetControlPoints()) {
        //     m_particles.push_back(std::make_shared<Particle>(p, 2, 10, false));
        // }
    }

    // qDebug() << "Particles: " << m_particles.size() << " Springs: " << m_springs.size() << " Triangle colliders: " << m_triangleColliders.size();
    
    // Add particles and springs to the physics system
    for (auto& p : m_particles) { m_physicsSystem.AddRigidbody(p); m_physicsSystem.AddConstraint(p); }
    for (auto& s : m_springs) m_physicsSystem.AddSpring(s);
    for (auto& t : m_triangleColliders) m_physicsSystem.AddTriangleCollider(t);

    m_physicsSystem.ChangeFriction(m_globalFriction);
    m_physicsSystem.RotateRigidbodies(m_globalRotation);

    m_physicsSystem.SetUpBVH();

    emit updateSpringsStiffnessControlsChanged(m_springs);
    
    doneCurrent();
    update();
    
}

void OpenGLWidget::LoadOBJ(const QString& filename)
{
    m_isCurve = false;
    m_model->LoadModel(filename);
    Reset();
}

void OpenGLWidget::SaveOBJ(const QString& filename)
{
    m_model->customOBJ->SaveOBJ(filename);
    // emit statusBarMessageChanged("Model saved");
}

void OpenGLWidget::InitCurves()
{
    m_curve.Clear();

    m_curve.AddControlPoint(QVector3D(0, 1, 0));
    m_curve.AddControlPoint(QVector3D(-0.4, 0.8, 0));
    m_curve.AddControlPoint(QVector3D(-0.4, 0.6, 0));
    m_curve.AddControlPoint(QVector3D(-1, 0, 0));
    m_curve.AddControlPoint(QVector3D(-0.8, -0.7, 0));
    m_curve.AddControlPoint(QVector3D(0, -1, 0));
    m_curve.AddControlPoint(QVector3D(0.8, -0.7, 0));
    m_curve.AddControlPoint(QVector3D(1, 0, 0));
    m_curve.AddControlPoint(QVector3D(0.4, 0.6, 0));
    m_curve.AddControlPoint(QVector3D(0.4, 0.8, 0));
    m_curve.AddControlPoint(QVector3D(0, 1, 0));

    m_curvePoints = m_curve.GetControlPoints();

    Reset();
}

void OpenGLWidget::CurveToParticlesSprings()
{
    if (!m_isCurve) return;

    makeCurrent();
    m_particles.clear();
    m_springs.clear();
    m_triangleColliders.clear();

    auto points = m_curve.Sample(m_numSamples);

    // Remove the last points to avoid duplicates
    if ((points.front() - points.back()).length() < 0.001f) points.pop_back(); 

    size_t numPoints = points.size();
    if (numPoints < 3) return; // Not enough points to create a curve

    // Compute the center of the curve
    QVector3D center(0, 0, 0);
    for (const auto& pt : points)
        center += pt;
    center /= float(numPoints);

    // Add offset to the center
    center.setY(center.y() - 0.2f);

    // Parameters
    float mass = 10.0f;
    int numLayers = m_curveLayers;
    float height = m_curveHeight;
    float layerStep = height / (numLayers - 1);
    float ringWidth = m_curveRingW;
    float ringHeight = m_curveRingH; 

    std::vector<std::vector<std::shared_ptr<Particle>>> layers;

    for (int layer = 0; layer < numLayers - 2; ++layer) 
    {
        std::vector<std::shared_ptr<Particle>> layerParticles;

        float t = float(layer) / float(numLayers - 2); 
        float z = layer * layerStep;

        float scale = sin((1.0f - t) * M_PI_2); 

        for (size_t i = 0; i < numPoints; ++i) {
            QVector3D pos = points[i];

            // Reduce the size of the layer
            QVector3D offset = pos - center;
            pos = center + offset * scale;
            pos.setZ(z);

            auto p = std::make_shared<Particle>(pos, 1, mass, layer != 0);
            layerParticles.push_back(p);
            m_particles.push_back(p);

            // Horizontally connect particles in the same layer
            if (i > 0) 
            {
                auto& p1 = layerParticles[i - 1];
                auto& p2 = layerParticles[i];
                float stiffness = GetStiffnessByQuadrant(p1->transform.position, p2->transform.position, center);
                auto spring = std::make_shared<Spring>(stiffness);
                spring->SetParticles(p1.get(), p2.get());
                m_springs.push_back(spring);
            }
        }

        // Close the curve
        auto& p1 = layerParticles.front();
        auto& p2 = layerParticles.back();
        float stiffness = GetStiffnessByQuadrant(p1->transform.position, p2->transform.position, center);
        auto springLoop = std::make_shared<Spring>(stiffness);
        springLoop->SetParticles(p1.get(), p2.get());
        m_springs.push_back(springLoop);

        layers.push_back(layerParticles);
    }

    // Last layer center
    QVector3D top(center.x(), center.y(), layerStep * (numLayers - 3) + layerStep * 0.5f);
    auto topParticle = std::make_shared<Particle>(top, 1, mass);
    m_particles.push_back(topParticle);

    // Penultimate layer (ring)
    std::vector<std::shared_ptr<Particle>> lastRing;

    for (size_t i = 0; i < numPoints; ++i) {
        float angle = float(i) / float(numPoints) * 2.0f * M_PI + M_PI_2;

        float x = top.x() + std::cos(angle) * ringWidth;
        float y = top.y() + std::sin(angle) * ringHeight;
        float z = top.z();

        QVector3D pos(x, y, z);
        auto p = std::make_shared<Particle>(pos, 1, mass);
        p->SetFlags(PARTICLE_NO_COLLISION_WITH_US);
        lastRing.push_back(p);
        m_particles.push_back(p);

        // Horizontal links
        if (i > 0) 
        {
            auto spring = std::make_shared<Spring>(1000); // Stiffness of ring springs
            spring->SetParticles(lastRing[i - 1].get(), lastRing[i].get());
            // spring->SetRigidity(true);
            m_springs.push_back(spring);
        }

    }

    // Close the ring
    auto ringLoop = std::make_shared<Spring>(1000);
    ringLoop->SetParticles(lastRing.front().get(), lastRing.back().get());
    // ringLoop->SetRigidity(true);
    m_springs.push_back(ringLoop);

    layers.push_back(lastRing);


    // Vertical and diagonal connections between layers
    for (int layer = 0; layer < numLayers - 2; ++layer) {
        for (size_t i = 0; i < numPoints; ++i) {
            auto& p1 = layers[layer][i];
            auto& p2 = layers[layer + 1][i];
            float stiffness = GetStiffnessByQuadrant(p1->transform.position, p2->transform.position, center);
            auto spring = std::make_shared<Spring>(stiffness);
            spring->SetParticles(p1.get(), p2.get());
            m_springs.push_back(spring);

            if (i > 0 && m_crossSpringModel) 
            {
                auto& p3 = layers[layer][i];
                auto& p4 = layers[layer + 1][i - 1];
                float stiffness = GetStiffnessByQuadrant(p3->transform.position, p4->transform.position, center);
                auto s1 = std::make_shared<Spring>(stiffness);
                s1->SetParticles(p3.get(), p4.get());
                m_springs.push_back(s1);
                
                auto& p5 = layers[layer][i - 1];
                auto& p6 = layers[layer + 1][i];
                stiffness = GetStiffnessByQuadrant(p5->transform.position, p6->transform.position, center);
                auto s2 = std::make_shared<Spring>(stiffness);
                s2->SetParticles(p5.get(), p6.get());
                m_springs.push_back(s2);
            }
        }

        if (m_crossSpringModel)
        {
            auto& p1 = layers[layer].back();
            auto& p2 = layers[layer + 1].front();
            float stiffness = GetStiffnessByQuadrant(p1->transform.position, p2->transform.position, center);
            auto s1 = std::make_shared<Spring>(stiffness);
            s1->SetParticles(p1.get(), p2.get());
            m_springs.push_back(s1);

            auto& p3 = layers[layer].front();
            auto& p4 = layers[layer + 1].back();
            stiffness = GetStiffnessByQuadrant(p3->transform.position, p4->transform.position, center);
            auto s2 = std::make_shared<Spring>(stiffness);
            s2->SetParticles(p3.get(), p4.get());
            m_springs.push_back(s2);
        }
        
    }

    // Connect the center to the last layer
    for (auto& p : layers.back()) {
        auto spring = std::make_shared<Spring>(1000); // Stiffness of ring springs
        spring->SetParticles(p.get(), topParticle.get());
        // spring->SetRigidity(true);
        m_springs.push_back(spring);
    }

    // Triangle collider generation
    for (int layer = 0; layer < numLayers - 2; ++layer)
    {
        const auto& current = layers[layer];
        const auto& next = layers[layer + 1];
        size_t count = current.size();

        for (size_t i = 0; i < count; ++i)
        {
            size_t next_i = (i + 1) % count;

            // First triangle of the quad
            auto a = current[i].get();
            auto b = next[i].get();
            auto c = next[next_i].get();

            // Second triangle of the quad
            auto d = current[next_i].get();

            m_triangleColliders.push_back(std::make_shared<TriangleCollider>(a, b, c));
            m_triangleColliders.push_back(std::make_shared<TriangleCollider>(a, c, d));
        }
    }

    const auto& lastLayer = layers.back(); // Ring layer
    for (size_t i = 0; i < lastLayer.size(); ++i)
    {
        size_t next_i = (i + 1) % lastLayer.size();

       auto a = lastLayer[i].get();
       auto b = lastLayer[next_i].get();
       auto c = topParticle.get();

        m_triangleColliders.push_back(std::make_shared<TriangleCollider>(a, b, c));
    }

    doneCurrent();
}

void OpenGLWidget::setDeformation(int p1, int p2, float value)
{
    if (!m_isCurve) return;

    p1--;
    p2--;

    QVector3D A = m_curvePoints[p1];
    QVector3D B = m_curvePoints[p2];

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

void OpenGLWidget::setHeight(float value)
{
    m_curveHeight = (value + 1.0f) * 0.5f * 2.0f;
    Reset();
}

void OpenGLWidget::setRing(float w, float h)
{
    m_curveRingW = (w + 1.0f) * 0.5f * 0.2f;
    m_curveRingH = (h + 1.0f) * 0.5f * 0.2f;

    qDebug() << "Ring W: " << m_curveRingW << " Ring H: " << m_curveRingH;

    Reset();
}
