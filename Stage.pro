# -------------------------------------------------
#                        App
# -------------------------------------------------

QT += core gui opengl widgets concurrent
win32: QT += openglwidgets

CONFIG += c++17 release
QMAKE_CXXFLAGS_RELEASE = -O3 -Wno-unused-parameter

TARGET = main

MOC_DIR = ./tmp/moc
OBJECTS_DIR = ./tmp/obj
UI_DIR = ./tmp/ui
RCC_DIR = ./tmp/qrc

SOURCES += \
    ./src/Main.cpp \
    ./libs/stb_image.cpp \
    ./src/core/Rigidbody.cpp \
    ./src/core/Camera.cpp \
    ./src/rendering/Render.cpp \
    ./src/rendering/Mesh.cpp \
    ./src/rendering/Model.cpp \
    ./src/rendering/Geometry3D.cpp \
    ./src/rendering/Curve.cpp \
    ./src/physics/Particle.cpp \
    ./src/physics/Spring.cpp \
    ./src/physics/CollisionSolver.cpp \
    ./src/physics/PhysicsSystem.cpp \
    ./src/physics/ModelPhysicsConverter.cpp \
    ./src/MainWindow.cpp \
    ./src/widgets/OpenglWidget.cpp \
    ./src/widgets/GlobalSettingsWidget.cpp \
    ./src/widgets/ModelSettingsWidget.cpp \
    ./src/widgets/SpringSettingsWidgets.cpp 

HEADERS += \
    ./libs/stb_image.h \
    ./src/core/Utils.h \
    ./src/core/Transform.h \
    ./src/core/BoundingBox.h \
    ./src/core/GeometryPrimitives.h \
    ./src/core/Rigidbody.h \
    ./src/core/BVH.h \
    ./src/core/Camera.h \
    ./src/rendering/Render.h \
    ./src/rendering/CustomOBJLoader.h \
    ./src/rendering/Mesh.h \
    ./src/rendering/Model.h \
    ./src/rendering/Geometry3D.h \
    ./src/rendering/Curve.h \
    ./src/physics/SphereCollider.h \
    ./src/physics/TriangleCollider.h \
    ./src/physics/Particle.h \
    ./src/physics/Spring.h \
    ./src/physics/CollisionSolver.h \
    ./src/physics/PhysicsSystem.h \
    ./src/physics/ModelPhysicsConverter.h \
    ./src/MainWindow.h \
    ./src/widgets/OpenglWidget.h \
    ./src/widgets/GlobalSettingsWidget.h \
    ./src/widgets/ModelSettingsWidget.h \
    ./src/widgets/SpringSettingsWidgets.h

#FORMS += 

INCLUDEPATH += \
    ./libs \
    ./resources \
    ./src \
    ./src/core \
    ./src/physics \
    ./src/rendering \
    ./src/widgets 
    #./libs/assimp-5.4.3/include

RESOURCES += $$PWD/resources/resources.qrc

win32: LIBS += -lopengl32 -lgdi32 #-L"libs/assimp-5.4.3/bin"
unix: LIBS += -lGL -ldl -lpthread #-lassimp
