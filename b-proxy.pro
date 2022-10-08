QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    config.cpp \
    main.cpp \
    proxy.cpp \
    widget_main.cpp

HEADERS += \
    config.h \
    proxy.h \
    widget_main.h

FORMS += \
    widget_main.ui
INCLUDEPATH += D:/code/boost/boost_1_80_0
CONFIG += debug_and_release

CONFIG(debug, debug|release){
LIBS += -LD:/code/boost/boost_1_80_0/stage/lib \
        -llibboost_json-vc142-mt-gd-x64-1_80 \
        -llibboost_random-vc142-mt-gd-x64-1_80 \
        -llibboost_filesystem-vc142-mt-gd-x64-1_80 \
        -lAdvapi32
} else {
LIBS += -LD:/code/boost/boost_1_80_0/stage/lib \
        -llibboost_json-vc142-mt-x64-1_80 \
        -llibboost_random-vc142-mt-x64-1_80 \
        -llibboost_filesystem-vc142-mt-x64-1_80 \
        -lAdvapi32
}
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
