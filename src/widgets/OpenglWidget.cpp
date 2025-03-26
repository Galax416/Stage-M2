#include "OpenglWidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), m_program(0)
{
    setFocusPolicy(Qt::StrongFocus);  // Permit to receive key events
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();
    delete m_program;
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
    m_camera->SetAspect(static_cast<float>(width) / static_cast<float>(height));

}

void OpenGLWidget::initShaders(QOpenGLShaderProgram *program, QString vertex_shader, QString fragment_shader)
{
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex_shader)) {
        qWarning() << "Error when compiling the vertex shader:" << program->log();
    }

    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment_shader)) {
        qWarning() << "Error when compiling the fragment shader:" << program->log();
    }

    if (!program->link()) {
        qWarning() << "Error when linking the shader program:" << program->log();
    }
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    m_program = new QOpenGLShaderProgram();
    initShaders(m_program, "./src/shaders/vertex_shader.vert", "./src/shaders/fragment_shader.frag");

    m_camera = new Camera(QVector3D(0.0f, 0.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f));

    m_physicsSystem.ClearAll();


    // Spring test 1 (pendulum)
    /*m_particles.push_back(Particle(QVector3D(0, 0, 0), 10, 10, false, QColor(0, 255, 0))); // m1
    m_particles.push_back(Particle(QVector3D(200, -100, 0), 10, 100, true, QColor(255, 0, 255))); // m2
    m_particles.push_back(Particle(QVector3D(-200, -100, 0), 10, 100, true, QColor(255, 0, 255))); // m3

    m_physicsSystem.AddRigidbody(&m_particles[0]);
    m_physicsSystem.AddRigidbody(&m_particles[1]);
    m_physicsSystem.AddRigidbody(&m_particles[2]);

    Spring spring1(15000.0f, 1.0f, 0.0f, QColor(255, 0, 0));
    spring1.SetParticles(&m_particles[0], &m_particles[1]);
    m_physicsSystem.AddSpring(spring1);

    Spring spring2(15000.0f, 1.0f, 0.0f, QColor(255, 0, 0));
    spring2.SetParticles(&m_particles[0], &m_particles[2]);
    m_physicsSystem.AddSpring(spring2);

    m_physicsSystem.ChangeFrictionParticle(1.0f);*/

    // Spring test 2 (double pendulum)
    /*m_particles.push_back(Particle(QVector3D(0, 0, 0), 10, 20, false));
    m_particles.push_back(Particle(QVector3D(-200, -200, -100), 10, 200, true));
    m_particles.push_back(Particle(QVector3D(0, -100, -100), 10, 20, true));

    m_physicsSystem.AddRigidbody(&m_particles[0]);
    m_physicsSystem.AddRigidbody(&m_particles[1]);
    m_physicsSystem.AddRigidbody(&m_particles[2]);

    Spring spring1(150000.0f, 1.0f, 0.0f);
    spring1.SetParticles(&m_particles[0], &m_particles[1]);
    
    Spring spring2(15000.0f, 1.0f, 0.0f);
    spring2.SetParticles(&m_particles[1], &m_particles[2]);

    m_physicsSystem.AddSpring(spring1);
    m_physicsSystem.AddSpring(spring2);

    m_physicsSystem.ChangeFrictionParticle(1.0f);*/

    // Spring test 3 (collision)
    /*m_particles.push_back(Particle(QVector3D(-50, 0, 0), 5, 10.0, false));
    m_particles.push_back(Particle(QVector3D(50, 0, 0), 5, 10.0, false));
    m_particles.push_back(Particle(QVector3D(-400, 0, 0), 10, 10.0, true, QColor(255, 0, 0)));
    m_particles.push_back(Particle(QVector3D(400, 0, 0), 10, 10.0, true, QColor(0, 0, 255)));

    m_physicsSystem.AddRigidbody(&m_particles[0]);
    m_physicsSystem.AddRigidbody(&m_particles[1]);
    m_physicsSystem.AddRigidbody(&m_particles[2]);
    m_physicsSystem.AddRigidbody(&m_particles[3]);

    // add constraints (collision particles between red and blue particules)
    m_physicsSystem.AddConstraint(&m_particles[2]);
    m_physicsSystem.AddConstraint(&m_particles[3]);

    m_physicsSystem.ChangeFrictionParticle(1.0f);

    Spring spring1(10000.0f, 1.0f, 0.0f);
    spring1.SetParticles(&m_particles[0], &m_particles[2]);

    Spring spring2(10000.0f, 1.0f, 0.0f);
    spring2.SetParticles(&m_particles[1], &m_particles[3]);

    m_physicsSystem.AddSpring(spring1);
    m_physicsSystem.AddSpring(spring2);*/

    // Spring test 4 (rope)
    /*int n = 30;
    float l = 20;

    m_particles.push_back(Particle(QVector3D(0, 0, 0), 2, 10.0, false));

    for (int i = 1; i < n; ++i) {
        m_particles.push_back(Particle(QVector3D(i*l+l, 0, 0), 2, 10.0, true));
    }
    for (int i = 1; i < n; ++i) {
        Spring spring(15000.0f, 0.0f, 0.0f);
        spring.SetParticles(&m_particles[i], &m_particles[i - 1]);
        m_physicsSystem.AddSpring(spring);
    }

    for (long unsigned int i = 0; i < m_particles.size(); i++) {
        m_physicsSystem.AddRigidbody(&m_particles[i]);
        m_physicsSystem.AddConstraint(&m_particles[i]);
    }*/

    // Spring test 5 (breast)
    int n = 15;
    float r = 1;
    float e = 0.2;
    int s1 = 110;
    int s2 = 70;
    int boule = 17;
    float bouleG = 30.0f;

    float k1a = 450;
    float k1b = 900;
    float k1c = 900;

    float k2a = 450 * 5;
    float k2b = 900 * 5;
    float k2c = 900 * 5;


    float t0 = M_PI / (n - 1);
    float n1 = static_cast<int>(0.5 + (s1 * M_PI / 180) / t0);
    float n2 = static_cast<int>(0.5 + (s2 * M_PI / 180) / t0);

    float l1 = 2 * r * sin(t0 / 2);
    float l2 = e;
    float l3 = 2 * (r + e) * sin(t0 / 2);

    qDebug() << l1 << " " << l2 << " " << l3;

    l1 = 0;
    l2 = 0;
    l3 = 0;

    // qDebug() << l1 << " " << l2 << " " << l3;

    // Set up the particles
    m_particles.push_back(Particle(QVector3D(0, r, 0), 5, 10.0, false, QColor(255, 0, 106)));

    for (int i = 1; i < n1; ++i) {
        float t = i * t0;
        float x = r * sin(t);
        float y = r * cos(t);
        m_particles.push_back(Particle(QVector3D(x, y, 0), 5, 10.0, true, QColor(255, 0, 0)));

    }
    for (int i = n1; i < n1+n2; ++i) {
        float t = i * t0;
        float x = r * sin(t);
        float y = r * cos(t);
        m_particles.push_back(Particle(QVector3D(x, y, 0), 5, 10.0, true, QColor(0, 0, 255)));
        
    }

    m_particles.push_back(Particle(QVector3D(0, -r, 0), 5, 10.0, false, QColor(255, 0, 106)));
    int m2 = m_particles.size() - 1;
    
    m_particles.push_back(Particle(QVector3D(0, r+e, 0), 5, 10.0, false, QColor(255, 0, 106)));
    int m3 = m_particles.size() - 1;

    for (int i = 1; i < n1; ++i) {
        float t = i * t0;
        float x = (r + e) * sin(t);
        float y = (r + e) * cos(t);
        m_particles.push_back(Particle(QVector3D(x, y, 0), 5, 10.0, true, QColor(255, 0, 0)));
    }
    for (int i = n1; i < n1 + n2; ++i) {
        float t = i * t0;
        float x = (r + e) * sin(t);
        float y = (r + e) * cos(t);
        m_particles.push_back(Particle(QVector3D(x, y, 0), 5, 10.0, true, QColor(0, 0, 255)));
    }

    m_particles.push_back(Particle(QVector3D(0, -r-e, 0), 5, 10.0, false, QColor(255, 0, 106)));
    int m4 = m_particles.size() - 1;

    float a = 0;
    while (a < r) {
        m_particles.push_back(Particle(QVector3D(0, a, 0), 5, 10.0, false, QColor(255, 0, 106)));
        a = a + e / 2;
    }

    float b = -e / 2;
    while (b > -r) {
        m_particles.push_back(Particle(QVector3D(0, b, 0), 5, 10.0, false, QColor(255, 0, 106)));
        b = b - e / 2;
    }

    // Set up the springs
    for(int i = 1; i < n1; ++i) {
        Spring spring(k1a, 1.0f, l1, QColor(255, 0, 0));
        spring.SetParticles(&m_particles[i], &m_particles[i-1]);
        m_physicsSystem.AddSpring(spring);
    }
    for(int i = n1; i < n1+n2; ++i) {
        Spring spring(k2a, 1.0f, l1, QColor(0, 0, 255));
        spring.SetParticles(&m_particles[i], &m_particles[i-1]);
        m_physicsSystem.AddSpring(spring);
    }
    Spring spring1(k2a, 1.0f, l1, QColor(0, 0, 255));
    spring1.SetParticles(&m_particles[m2], &m_particles[n1+n2-1]);
    m_physicsSystem.AddSpring(spring1);

    for (int i = 1; i < n1; ++i) {
        Spring spring1(k1c, 1.0f, l3, QColor(255, 0, 0));
        spring1.SetParticles(&m_particles[i + n], &m_particles[i + n - 1]);
        m_physicsSystem.AddSpring(spring1);
        Spring spring2(k1b, 1.0f, l2, QColor(255, 0, 0));
        spring2.SetParticles(&m_particles[i + n], &m_particles[i]);
        m_physicsSystem.AddSpring(spring2);
        // Spring spring3(k1c, 1.0f, l3, QColor(255, 0, 0));
        // spring3.SetParticles(&m_particles[i + n], &m_particles[i + 1]);
        // m_physicsSystem.AddSpring(spring3);
        // Spring spring4(k1b, 1.0f, l2, QColor(255, 0, 0));
        // spring4.SetParticles(&m_particles[i + n + 1], &m_particles[i]);
        // m_physicsSystem.AddSpring(spring4);
    }
    // Spring s8(k1c, 1.0f, l3, QColor(255, 0, 0));
    // s8.SetParticles(&m_particles[0], &m_particles[n+1]);
    // m_physicsSystem.AddSpring(s8);
    // Spring s9(k1a, 1.0f, l1, QColor(255, 0, 0));
    // s9.SetParticles(&m_particles[1], &m_particles[n]);
    // m_physicsSystem.AddSpring(s9);
    for (int i = n1; i < n1 + n2; ++i) {
        Spring spring1(k2c, 1.0f, l3, QColor(0, 0, 255));
        spring1.SetParticles(&m_particles[i + n], &m_particles[i + n - 1]);
        m_physicsSystem.AddSpring(spring1);
        Spring spring2(k2b, 0.0f, l2, QColor(0, 0, 255));
        spring2.SetParticles(&m_particles[i + n], &m_particles[i]);
        m_physicsSystem.AddSpring(spring2);
        // Spring spring3(k2c, 1.0f, l3, QColor(0, 0, 255));
        // spring3.SetParticles(&m_particles[i + n], &m_particles[i + 1]);
        // m_physicsSystem.AddSpring(spring3);
        // Spring spring4(k2b, 1.0f, l2, QColor(0, 0, 255));
        // spring4.SetParticles(&m_particles[i + n + 1], &m_particles[i]);
        // m_physicsSystem.AddSpring(spring4);
    }
    Spring spring2(k2c, 1.0f, l3, QColor(0, 0, 255));
    spring2.SetParticles(&m_particles[m4], &m_particles[2*(n1+n2)]);
    m_physicsSystem.AddSpring(spring2);

    Spring spring3(0, 1.0f, 2*r+2*e, QColor(255, 0, 106));
    spring3.SetParticles(&m_particles[m3], &m_particles[m4]);
    m_physicsSystem.AddSpring(spring3);

    // Set up particles in
    m_particles.push_back(Particle(QVector3D(0.30, 0, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(0.30, -0.30, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(0.30, 0.30, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(0.60, 0, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(0.60, -0.30, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(0.60, 0.30, 0), boule, bouleG, true, QColor(255, 0, 106)));

    // Add particles to physics system
    for (long unsigned int i = 0; i < m_particles.size(); i++) {
        m_physicsSystem.AddRigidbody(&m_particles[i]);
        m_physicsSystem.AddConstraint(&m_particles[i]);
    }

    /*m_particles.push_back(Particle(QVector3D(0, 0, 0), 30, 10, true, QColor(255, 255, 255))); // Base
    m_particles.push_back(Particle(QVector3D(1, 0, 0), 30, 10, false, QColor(255, 0, 0))); // x
    m_particles.push_back(Particle(QVector3D(0, 1, 0), 30, 10, false, QColor(0, 0, 255))); // y
    m_particles.push_back(Particle(QVector3D(0, 0, 1), 30, 10, false, QColor(0, 255, 0))); // z

    m_physicsSystem.AddRigidbody(&m_particles[0]);
    m_physicsSystem.AddRigidbody(&m_particles[1]);
    m_physicsSystem.AddRigidbody(&m_particles[2]);
    m_physicsSystem.AddRigidbody(&m_particles[3]);

    Spring spring1(100.0f, 1.0f, 0.0f, QColor(255, 0, 0));
    spring1.SetParticles(&m_particles[0], &m_particles[1]);

    Spring spring2(100.0f, 1.0f, 0.0f, QColor(0, 0, 255));
    spring2.SetParticles(&m_particles[0], &m_particles[2]);

    Spring spring3(100.0f, 1.0f, 0.0f, QColor(0, 255, 0));
    spring3.SetParticles(&m_particles[0], &m_particles[3]);

    m_physicsSystem.AddSpring(spring1);
    m_physicsSystem.AddSpring(spring2);
    m_physicsSystem.AddSpring(spring3);*/

    // Scene particles-ground
    /*m_particles.push_back(Particle(QVector3D(0, 0, 0), 10, 10, true, QColor(200, 0, 200)));
    m_physicsSystem.AddRigidbody(&m_particles[0]);

    Plane* plane = new Plane(QVector3D(0, -2, 0), QVector3D(0, 1, 0));
    m_physicsSystem.AddRigidbody(plane);*/


    


}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program->programId());

    m_program->bind();
    m_program->setUniformValue("projection", m_camera->GetProjectionMatrix());
    m_program->setUniformValue("view", m_camera->GetViewMatrix());

    m_physicsSystem.Render(m_program);

    m_program->release();
    
    if (!m_isPaused) {
        emit statusBarMessageChanged("Simulation running...");
        m_physicsSystem.Update(deltaTime);
    } else emit statusBarMessageChanged("");

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
        m_isPaused = !m_isPaused;
        break;
    
    default:
        break;
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    
}