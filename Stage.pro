########################################
################# App ################## 
########################################

QT += core gui opengl widgets 
win32: QT += openglwidgets

CONFIG += c++11 release
QMAKE_CXXFLAGS_RELEASE = -O3 -Wno-unused-parameter

TARGET = main

MOC_DIR = ./tmp/moc
OBJECTS_DIR = ./tmp/obj
UI_DIR = ./tmp/ui

SOURCES += ./src/Main.cpp \
           ./src/MainWindow.cpp \
           ./src/Geometry.cpp \
           ./src/Particle.cpp \
           ./src/Spring.cpp \
           ./src/PhysicsSystem.cpp \
           ./src/widgets/OpenglWidget.cpp

HEADERS += ./src/Constants.h \
           ./src/MainWindow.h \
           ./src/Geometry.h \
           ./src/Rigidbody.h \
           ./src/Particle.h \
           ./src/Spring.h \
           ./src/PhysicsSystem.h \
           ./src/Utils.h \
           ./src/widgets/OpenglWidget.h

#FORMS += 

INCLUDEPATH += ./libs \
               ./src \
               ./src/widgets \
               ./src/shaders

win32: LIBS += -lopengl32