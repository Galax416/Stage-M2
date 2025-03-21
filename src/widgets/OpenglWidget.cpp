#include "OpenglWidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), m_program(0), m_shader(0)
{
    setFocusPolicy(Qt::StrongFocus);  // Permit to receive key events
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();
    delete m_program;
    delete m_shader;

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

}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram();

    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "./src/shaders/shader.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "./src/shaders/shader.frag");
    m_program->link();

    m_physicsSystem.ClearRigidbodys();
    m_physicsSystem.ClearSprings();
    m_physicsSystem.ClearConstraints();

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
    m_physicsSystem.AddSpring(spring2);*/

    // Spring test 2 (double pendulum)
    /*m_particles.push_back(Particle(QVector3D(0, 0, 0), 10, 20, false));
    m_particles.push_back(Particle(QVector3D(-200, -200, -100), 10, 20, true));
    m_particles.push_back(Particle(QVector3D(0, -100, -100), 10, 20, true));

    m_physicsSystem.AddRigidbody(&m_particles[0]);
    m_physicsSystem.AddRigidbody(&m_particles[1]);
    m_physicsSystem.AddRigidbody(&m_particles[2]);

    Spring spring1(15000.0f, 1.0f, 0.0f);
    spring1.SetParticles(&m_particles[0], &m_particles[1]);
    
    Spring spring2(15000.0f, 1.0f, 0.0f);
    spring2.SetParticles(&m_particles[1], &m_particles[2]);

    m_physicsSystem.AddSpring(spring1);
    m_physicsSystem.AddSpring(spring2);*/

    // Spring test 3 (collision)
    /*m_particles.push_back(Particle(QVector3D(-50, 0, 0), 5, 10.0, false));
    m_particles.push_back(Particle(QVector3D(50, 0, 0), 5, 10.0, false));
    m_particles.push_back(Particle(QVector3D(-400, 0, 0), 10, 10.0, true, QColor(255, 0, 0)));
    m_particles.push_back(Particle(QVector3D(400, 0, 0), 10, 10.0, true, QColor(0, 0, 255)));

    m_physicsSystem.AddRigidbody(&m_particles[0]);
    m_physicsSystem.AddRigidbody(&m_particles[1]);
    m_physicsSystem.AddRigidbody(&m_particles[2]);
    m_physicsSystem.AddRigidbody(&m_particles[3]);

    m_physicsSystem.AddConstraint(&m_particles[2]);
    m_physicsSystem.AddConstraint(&m_particles[3]);

    m_physicsSystem.ChangeFrictionParticle(0.999f);

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
        Spring spring(10000.0f, 1.0f, 0.0f);
        spring.SetParticles(&m_particles[i], &m_particles[i - 1]);
        m_physicsSystem.AddSpring(spring);
    }

    for (long unsigned int i = 0; i < m_particles.size(); i++) m_physicsSystem.AddRigidbody(&m_particles[i]);
    m_physicsSystem.ChangeFrictionParticle(0.99f);*/
 
    // Spring test 5 (breast)
    // On définit les valeurs
    int n = 15;
    int r = 300;
    int e = 75;
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

    int a = 0;
    while (a < r) {
        m_particles.push_back(Particle(QVector3D(0, a, 0), 5, 10.0, false, QColor(255, 0, 106)));
        a = a + e / 2;
    }

    int b = -e / 2;
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
    }
    for (int i = n1; i < n1 + n2; ++i) {
        Spring spring1(k2c, 1.0f, l3, QColor(0, 0, 255));
        spring1.SetParticles(&m_particles[i + n], &m_particles[i + n - 1]);
        m_physicsSystem.AddSpring(spring1);
        Spring spring2(k2b, 0.0f, l2, QColor(0, 0, 255));
        spring2.SetParticles(&m_particles[i + n], &m_particles[i]);
        m_physicsSystem.AddSpring(spring2);
        // Spring spring3(k1c, 1.0f, l3, QColor(0, 0, 255));
        // spring3.SetParticles(&m_particles[i + n], &m_particles[i + 1]);
        // m_physicsSystem.AddSpring(spring3);
    }
    Spring spring2(k2c, 1.0f, l3, QColor(0, 0, 255));
    spring2.SetParticles(&m_particles[m4], &m_particles[2*(n1+n2)]);
    m_physicsSystem.AddSpring(spring2);

    Spring spring3(0, 1.0f, 2*r+2*e, QColor(255, 0, 106));
    spring3.SetParticles(&m_particles[m3], &m_particles[m4]);
    m_physicsSystem.AddSpring(spring3);

    // Set up particles in
    m_particles.push_back(Particle(QVector3D(75, 0, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(75, -120, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(75, 120, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(160, 0, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(160, -120, 0), boule, bouleG, true, QColor(255, 0, 106)));
    m_particles.push_back(Particle(QVector3D(160, 120, 0), boule, bouleG, true, QColor(255, 0, 106)));

    // Add particles to physics system
    for (long unsigned int i = 0; i < m_particles.size(); i++) {
        m_physicsSystem.AddRigidbody(&m_particles[i]);
        m_physicsSystem.AddConstraint(&m_particles[i]);
    }



}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    m_physicsSystem.Render(m_program);

    m_program->release();
    
    if (!m_isPaused) {
        m_physicsSystem.Update(deltaTime);
    }

    update();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    
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