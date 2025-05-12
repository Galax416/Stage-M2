#pragma once

#include <QObject>
#include <QTimer>
#include <memory>

#include "PhysicsSystem.h"
#include "OpenGLWidget.h"

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
        m_timer->start(m_deltaTime * 1000); // Convert seconds to milliseconds
    }

    void Stop() 
    {
        if (m_timer) if (m_timer->isActive()) m_timer->stop();
        m_running = false;
    }
    
    void Update()
    {
        if (!m_running) return;

        if (m_physicsSystem) {
            // QMutexLocker locker(&mutex);
            m_physicsSystem->Update(m_deltaTime);
            emit Updated();
        }
    }

    void SetPhysicsRunning(bool running) 
    { 
        m_running = running;
    }
    
    void SetDeltaTime(float deltaTime) 
    { 
        Stop(); // Stop the timer before changing the delta time
        m_deltaTime = deltaTime; 
        Start(); // Restart the timer with the new delta time
    } 

    // void Shutdown() { 
    //     Stop();
    //     if (m_timer) {
    //         m_timer->deleteLater(); // Delete the timer when shutting down
    //         m_timer = nullptr;
    //     }
    //     this->deleteLater(); // Delete the worker when shutting down
    // }
    

signals:
    void Updated();

private :
    std::shared_ptr<PhysicsSystem> m_physicsSystem { nullptr };
    QTimer *m_timer;
    float m_deltaTime { DeltaTime };
    bool m_running  { false }; // Default to false, so the physics system is not running until explicitly started

};
