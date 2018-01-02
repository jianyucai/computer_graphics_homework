#-------------------------------------------------
#
# Project created by QtCreator 2017-12-18T21:46:06
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MinimalSurfaces
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        ArcBall.cpp \
    renderingwidget.cpp \
    ./HE_mesh/Mesh3D.cpp \
    minimal_surface.cpp

HEADERS += \
        mainwindow.h \
    renderingwidget.h \
    ArcBall.h \
    globalFunctions.h \
    ./HE_mesh/Mesh3D.h \
    ./HE_mesh/Vec.h \
    minimal_surface.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += \
        Eigen \

LIBS+= -framework opengl -framework glut



mac: LIBS += -framework GLUT
else:unix|win32: LIBS += -lGLUT
