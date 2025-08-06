# -------------------------------------------------
#                        App
# -------------------------------------------------

QT += core gui opengl widgets concurrent
win32: QT += openglwidgets

CONFIG += c++11 release console
QMAKE_CXXFLAGS += -pipe -std=c++17 -fopenmp -O3 -Wno-unused-parameter -isystem ./libs/eigen-3.4.0 -isystem ./libs/boost_1_78_0 -isystem ./libs/CGAL-5.6.1/include

TARGET = main

MOC_DIR = ./tmp/moc
OBJECTS_DIR = ./tmp/obj
UI_DIR = ./tmp/ui
RCC_DIR = ./tmp/qrc

SOURCES += \
    ./src/Main.cpp \
    ./libs/stb_image.cpp \
    ./src/core/Intersections.cpp \
    ./src/core/Rigidbody.cpp \
    ./src/core/Trackball.cpp \
    ./src/core/Camera.cpp \
    ./src/rendering/Render.cpp \
    ./src/rendering/Mesh.cpp \
    ./src/rendering/Model.cpp \
    ./src/rendering/Geometry3D.cpp \
    ./src/rendering/Curve.cpp \
    ./src/rendering/Voxel.cpp \
    ./src/physics/Particle.cpp \
    ./src/physics/Spring.cpp \
    ./src/physics/CollisionSolver.cpp \
    ./src/physics/PhysicsSystem.cpp \
    ./src/utils/ModelPhysicsConverter.cpp \
    ./src/MainWindow.cpp \
    ./src/widgets/OpenglWidget.cpp \
    ./src/widgets/GlobalSettingsWidget.cpp \
    ./src/widgets/ModelSettingsWidget.cpp \
    ./src/widgets/SpringSettingsWidgets.cpp 

HEADERS += \
    ./libs/stb_image.h \
    ./src/core/Transform.h \
    ./src/core/BoundingBox.h \
    ./src/core/SphereCollider.h \
    ./src/core/TriangleCollider.h \
    ./src/core/GeometryPrimitives.h \
    ./src/core/Intersections.h \
    ./src/core/Rigidbody.h \
    ./src/core/BVH.h \
    ./src/core/Trackball.h \
    ./src/core/Camera.h \
    ./src/rendering/Render.h \
    ./src/rendering/Mesh.h \
    ./src/rendering/Model.h \
    ./src/rendering/Geometry3D.h \
    ./src/rendering/Curve.h \
    ./src/rendering/Voxel.h \
    ./src/physics/Particle.h \
    ./src/physics/Spring.h \
    ./src/physics/CollisionSolver.h \
    ./src/physics/PhysicsSystem.h \
    ./src/physics/PhysicsWorker.h \
    ./src/physics/MaterialPropierties.h \
    ./src/utils/Utils.h \
    ./src/utils/CustomOBJLoader.h \
    ./src/utils/ModelPhysicsConverter.h \
    ./src/utils/Remeshing.h \
    ./src/MainWindow.h \
    ./src/widgets/OpenglWidget.h \
    ./src/widgets/GlobalSettingsWidget.h \
    ./src/widgets/ModelSettingsWidget.h \
    ./src/widgets/SpringSettingsWidgets.h

#FORMS += 

INCLUDEPATH += \
    ./libs \
    ./libs/eigen-3.4.0 \
    ./libs/boost_1_78_0 \
    ./libs/CGAL-5.6.1/include \
    ./resources \
    ./src \
    ./src/core \
    ./src/physics \
    ./src/rendering \
    ./src/utils \
    ./src/widgets 

RESOURCES += $$PWD/resources/resources.qrc

win32: LIBS += -lopengl32 -lgdi32
unix: LIBS += -lGL -ldl -lpthread
