#-------------------------------------------------
#
# Project created by QtCreator 2014-09-26T10:30:30
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProcessCam
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    Calibration.cpp \
    Camera.cpp \
    Add_Class/time_catch.cpp \
    morphological.cpp

HEADERS  += mainwindow.h \
    Calibration.h \
    Camera.h \
    Add_Class/time_catch.h \
    morphological.h

FORMS    += mainwindow.ui


INCLUDEPATH += /usr/local/include/opencv2
LIBS += -L/usr/local/lib
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_contrib
LIBS += -lopencv_legacy
LIBS += -lopencv_flann
LIBS += -L/usr/lib/x86_64-linux-gnu -lcurl


