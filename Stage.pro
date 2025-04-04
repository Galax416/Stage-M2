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
RCC_DIR = ./tmp/qrc

SOURCES += \
    ./src/Main.cpp \
    ./src/stb_image.cpp \
    ./src/Collision.cpp \
    ./src/Rigidbody.cpp \
    ./src/Particle.cpp \
    ./src/Spring.cpp \
    ./src/PhysicsSystem.cpp \
    ./src/Camera.cpp \
    ./src/Mesh.cpp \
    ./src/Model.cpp \
    ./src/Geometry.cpp \
    ./src/MainWindow.cpp \
    ./src/widgets/OpenglWidget.cpp \
    ./src/widgets/GlobalSettingsWidget.cpp 

HEADERS += \
    ./src/Constants.h \
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
    ./src/CustomOBJLoader.h \
    ./src/stb_image.h \
    ./src/MainWindow.h \
    ./src/widgets/OpenglWidget.h \
    ./src/widgets/GlobalSettingsWidget.h \

#FORMS += 

INCLUDEPATH += \
    ./libs \
    ./resources \
    ./src \
    ./src/widgets 
    #./libs/assimp-5.4.3/include

RESOURCES += $$PWD/resources/resources.qrc

win32: LIBS += -lopengl32 -lgdi32 #-L"libs/assimp-5.4.3/bin"
unix: LIBS += -lGL -ldl -lpthread #-lassimp
