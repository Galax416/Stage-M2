########################################
################# App ################## 
########################################

QT += core gui opengl widgets 
win32: QT += openglwidgets

CONFIG += c++11 release
QMAKE_CXXFLAGS_RELEASE = -O3 -Wno-unused-parameter

TARGET = App

MOC_DIR = ./tmp/moc
OBJECTS_DIR = ./tmp/obj

SOURCES += ./src/Main.cpp \
           ./src/Mainwindow.cpp \
           ./src/Geometry.cpp \
           ./src/Particle.cpp \
           ./src/Spring.cpp \
           ./src/PhysicsSystem.cpp \
           ./src/widgets/OpenglWidget.cpp

HEADERS += ./src/Constants.h \
           ./src/Mainwindow.h \
           ./src/Geometry.h \
           ./src/Rigidbody.h \
           ./src/Particle.h \
           ./src/Spring.h \
           ./src/PhysicsSystem.h \
           ./src/Utils.h \
           ./src/widgets/OpenglWidget.h


INCLUDEPATH += ./libs \
               ./src \
               ./src/widgets \
               ./src/shaders

