#-------------------------------------------------
#
# Project created by QtCreator 2019-01-12T12:27:11
#
#-------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

unix {
    LIBS += -lGLU
}
win32 {
    LIBS += -lGLU32 -lOpengl32
}

TARGET = projetPGRA
TEMPLATE = app


SOURCES += main.cpp\
        cogwheel.cpp \
        cylinder.cpp \
        link.cpp \
        polyedra.cpp \
        princ.cpp \
        glarea.cpp \
        settingsdialog.cpp

HEADERS  += princ.h \
        cogwheel.h \
        cylinder.h \
        glarea.h \
        link.h \
        polyedra.h \
        settingsdialog.h

FORMS    += princ.ui \
        settingsdialog.ui

RESOURCES += \
    projetPGRA.qrc
