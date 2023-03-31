TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

# opencv
INCLUDEPATH +="/usr/local/opencv/include/"
LIBS += -L"/usr/local/opencv/lib/"
LIBS += -lopencv_core \
        -lopencv_features2d \
        -lopencv_highgui \
        -lopencv_imgcodecs \
        -lopencv_imgproc \
        -lopencv_video \
        -lopencv_videoio \
        -lopencv_videostab

DISTFILES += \
    trash
