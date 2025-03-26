# -------------------------------------------------
#                        App
# -------------------------------------------------

QT += core gui opengl widgets 
win32: QT += openglwidgets

CONFIG += c++17 release
QMAKE_CXXFLAGS_RELEASE = -O3 -Wno-unused-parameter

TARGET = main

MOC_DIR = ./tmp/moc
OBJECTS_DIR = ./tmp/obj
UI_DIR = ./tmp/ui

SOURCES += ./src/Main.cpp \
           ./src/MainWindow.cpp \
           ./src/stb_image.cpp \
           ./src/Particle.cpp \
           ./src/Spring.cpp \
           ./src/PhysicsSystem.cpp \
           ./src/Camera.cpp \
           ./src/Mesh.cpp \
           ./src/Model.cpp \
           ./src/Geometry.cpp \
           ./src/widgets/OpenglWidget.cpp 

HEADERS += ./src/Constants.h \
           ./src/MainWindow.h \
           ./src/stb_image.h \
           ./src/Utils.h \
           ./src/Transform.h \
           ./src/Collision.h \
           ./src/Rigidbody.h \
           ./src/Particle.h \
           ./src/Spring.h \
           ./src/PhysicsSystem.h \
           ./src/Camera.h \
           ./src/Mesh.h \
           ./src/Model.h \
           ./src/Geometry.h \
           ./src/widgets/OpenglWidget.h

#FORMS += 

INCLUDEPATH += ./libs \
               ./src \
               ./src/widgets \
               ./src/shaders \
               ./libs/assimp-5.4.3/include

win32: LIBS += -lopengl32 -lgdi32 -L"libs/assimp-5.4.3/bin"
unix: LIBS += -lGL -ldl -lpthread -lassimp
