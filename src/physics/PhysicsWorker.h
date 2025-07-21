#pragma once

#include <QObject>
#include <QTimer>
#include <memory>

#include "PhysicsSystem.h"
// #include "OpenGLWidget.h"

#define DELTATIME  0.0100f

class PhysicsWorker : public QObject
{
    Q_OBJECT

public:
    // QMutex mutex; // Mutex for thread safety

    explicit PhysicsWorker(std::shared_ptr<PhysicsSystem> physicsSystem, QObject* parent = nullptr) 
        : QObject(parent), m_physicsSystem(physicsSystem), m_running(false) 
    {
        if (!m_physicsSystem) {
            qWarning() << "Warning: PhysicsWorker initialized with a nullptr PhysicsSystem!";
        }

        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &PhysicsWorker::Update);
    }

public slots:
    void Start() 
    {
        if (!m_timer->isActive()) m_timer->start(m_deltaTime * 1000); // Convert seconds to milliseconds
    }

    void Stop() 
    {
        if (m_timer) if (m_timer->isActive()) m_timer->stop();
        m_running = false;

        while (m_updating.loadAcquire() == 1) QThread::msleep(1); // Wait until the update is finished
    }
    
    void Update()
    {
        if (!m_running) return;
        if (m_updating.fetchAndStoreRelaxed(1) == 1) return;

        if (m_physicsSystem) 
        {
            m_physicsSystem->Update(m_deltaTime);
            emit Updated();
        }
        m_updating.storeRelease(0);
    }

    void SetPhysicsRunning(bool running) 
    { 
        m_running = running;
        if (m_running) Start();
        else Stop();
    }
    
    void SetDeltaTime(float deltaTime) 
    { 
        Stop(); // Stop the timer before changing the delta time
        m_deltaTime = deltaTime; 
        Start(); // Restart the timer with the new delta time
    }

    // bool IsUpdating() const { return m_updating.loadRelaxed() == 1; }

signals:
    void Updated();

private :
    std::shared_ptr<PhysicsSystem> m_physicsSystem { nullptr };
    QTimer *m_timer;
    float m_deltaTime { DELTATIME };
    bool m_running  { false }; // Default to false, so the physics system is not running until explicitly started
    
    QAtomicInt m_updating { 0 }; // 0 = free, 1 = busy
};
